#include "program.h"
#include "../ASCII/ASCII_ctrl.h"
#include "../tool/convert.h"

const char program_c::PROGRAM_FILE_EXTENSION[]=	"PRG";
const char program_c::ERROR_FILE_EXTENSION[]=	"ERR";
const char program_c::LOG_FILE_EXTENSION[]=		"LOG";
const char program_c::PARSE_FILE_EXTENSION[]=	"PRS";

const char program_c::ERROR_TEXT[]=	"ok\0"
									"line to long\0"	
									"unknown command\0"
									"invalid label char\0"
									"label to long\0"
									"delay to long >60s\0"
									"invalid delay\0"
									"previous expression pending\0"
									"variable previosly used\0"
									"multiple operators selected\0"
									"no variable selected\0"
									"immedeate assignement only\0"
									"invalid parameter sequence\0"
									"multiple conditions not allowed\0"
									"invalid operator\0"
									"no operator selected\0"
									"out of message memory\0"
									"unexpected extra data\0"
									"flag used without index\0"
									"closing loop without opening loop\0"
									"label has been previoulsly defined\0"
									"referenced label is undefined\0" 
									"opening loop without closing loop\0"
									"expected data\0"
									"\0\0";

const char program_c::CMD_TEXT[]=	"SET\0"
									"WAIT\0"
									"LOOP\0"
									"JUMP\0"
									"#\0"
									":\0"
									"LOG\0"
									"DISP\0"
									"INFO\0"
									"PASS\0"
									"FAIL\0"
									"CLEAR\0"
									"LOAD\0"
									"SAVE\0"
									"\0\0";
									
const char program_c::LABEL_CHARS[]="0123456789"					//  0... 9
									"@ABCDEFGHIJKLMNOPQRSTUVWXYZ"	// 10...36
									"_abcdefghijklmnopqrstuvwxyz";	// 37...63
									
/* *********************************************************************
    ! 0 ! 1 ! 2 ! 3 ! 4 ! 5 ! 6 ! 7 ! 8 ! 9 ! A ! B ! C ! D ! E ! F !
  --+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  0 ! 0 ! 1 ! 2 ! 3 ! 4 ! 5 ! 6 ! 7 ! 8 ! 9 ! @ ! A ! B ! C ! D ! E !    
  --+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  1 ! F ! G ! H ! I ! J ! K ! L ! M ! N ! O ! P ! Q ! R ! S ! T ! U !    
  --+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  2 ! V ! W ! X ! Y ! Z ! _ ! a ! b ! c ! d ! e ! f ! g ! h ! i ! j !    
  --+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  3 ! k ! l ! m ! n ! o ! p ! q ! r ! s ! t ! u ! v ! w ! x ! y ! z !    
  --+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
   ********************************************************************* */

const char program_c::JUMP_CHARS[]=	"UIORF:<=>";
const char program_c::SET_CHARS[]=	"UIOF=+-";

program_c::program_c(controller_c * Controller, parameter * Parameter)
{	// program_c
	process_state=INIT;
	pController=Controller;
	pParameter=Parameter;
	verbose=true;
}	// program_c


uint16_t 	program_c::getFileCount(void)
{	// get file cont
	return file_cnt;
}	// get file cont

char * 		program_c::getFileName(uint16_t FileNr)
{	//	get file name
	char * name=temp_file_name;

	if (FileNr)
	{	// not zero, search for file
		if (FileNr==temp_file_number)
		{	// current temp_file
			name=temp_file_name;
		}	// current temp_file
		else if (FileNr==file_number)
		{	// current program file name
			name=file_name;
		}	// current program file name
		else if (FileNr<=file_cnt)
		{	// get file name
			if (get_file_name(FileNr))
			{	// file exists
				temp_file_number=FileNr;
				cpy_file_name(temp_file_name,get_file_name());
			}	// file exists
			else
			{	// does not exist
				name=NULL;
			}	// does not exist
		}	// get file name
		else
		{	// does not exist
			name=NULL;
		}	// does not exist
	}	// not zero, search for file
	return name;
}	//	get file name

uint16_t 	program_c::getProgramFileNumber(void)
{	// number program file
	return file_number;
}	// number program file

void		program_c::setProgramFileNumber(uint16_t FileNr)
{	// setProgramFileNumber
	if (FileNr)
	{	// program file selected
		char * name=getFileName(FileNr);
		if (name)
		{	// exists
			file_number=FileNr;
			cpy_file_name(file_name,temp_file_name);
			process_state=OPEN_FILE;
			
		}	// exists
		else
		{	// does not exist
			file_number=0;
			file_name[0]=NUL;
			temp_file_name[0]=NUL;
			temp_file_number=0;
			clear_program_data();
		}	// does not exist
	}	// program file selected
	else
	{	// program disabled
			file_number=0;
			file_name[0]=NUL;
			temp_file_name[0]=NUL;
			temp_file_number=0;
			clear_program_data();
	}	// program disabled
}	// setProgramFileNumber

void		program_c::setProgramFileName(const char * pFileName)
{	// setProgramFileName
	set_search_name(pFileName);
	process_state=FIND_FILE;
}	// setProgramFileName

bool		program_c::process(void)
{	//	process 
	switch (process_state)
	{	// switch process_state
		case INIT:
			file_cnt=0;
			file_number=0;
			file_name[0]=NUL;
			temp_file_number=0;
			temp_file_name[0]=NUL;
			set_extension_filter(PROGRAM_FILE_EXTENSION);
			set_name_filter("");
			if (hasSD())
			{	// ok, go on
				process_state=COUNT_FILES;
			}	// ok, go on
			else
			{	// no card, end
				process_state=NO_CARD;
			}	// no card, end
			break;
		case IDLE:
			if (!hasSD())
			{	// SD-Card has been removed
				clear_program_data();
				process_state=NO_CARD;
			}	// SD-Card has been removed
			break;
		case COUNT_FILES:
			file_cnt=get_file_cnt();
			if (!dir_c::isBusy())
			{	// done, next step
				process_state=IDLE;
			}	// done, next step
			break;
		case FIND_FILE:
			if (!find_file())
			{	// get file number and name
				file_number=get_current_file_number();
				if (file_number)
				{	// found
					cpy_file_name(file_name,get_file_name());
					process_state=OPEN_FILE;
				}	// found
				else
				{	// not found
					file_name[0]=0;
					process_state=IDLE;
				}	// not found
				set_search_name("");
			}	// get file number and name
			break;
		case OPEN_FILE:
			clear_program_data();
			
			add_file_ext(program_file_name_ext,file_name,PROGRAM_FILE_EXTENSION);
			add_file_ext(error_file_name_ext,file_name,ERROR_FILE_EXTENSION);
			add_file_ext(log_file_name_ext,file_name,LOG_FILE_EXTENSION);
			add_file_ext(parse_file_name_ext,file_name,PARSE_FILE_EXTENSION);

			program_file=SD.open(program_file_name_ext,FILE_READ);
			if (SD.exists(error_file_name_ext))
				SD.remove(error_file_name_ext);
			error_file=SD.open(error_file_name_ext,FILE_WRITE);
			log_file=SD.open(log_file_name_ext,FILE_WRITE);
			
			if (verbose)
			{ 	// verbose
				parse_file=SD.open(parse_file_name_ext,FILE_WRITE);
				parse_file.println("***********************************************");
				parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
				parse_file.print(": log file opened "); parse_file.println(log_file_name_ext);
				parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
				parse_file.print(": error file opened "); parse_file.println(error_file_name_ext);
				parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
				parse_file.print(": prarser file opened "); parse_file.println(parse_file_name_ext);
				parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
				parse_file.print(": Start load file "); parse_file.println(program_file_name_ext);
			} 	// verbose

			process_state=LOAD_FILE;
			break;
		case LOAD_FILE:
			if (!read_program_file_line())
			{	// eof reached
				process_state=PARSE_LABEL;
				psw.program_data_length=psw.program_data_element;

				if (verbose)
				{ 	// verbose
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.print(": End load file, ");
					parse_file.print(psw.program_lines);
					parse_file.print(" lines processed, ");
					parse_file.print(psw.program_data_length);
					parse_file.println(" PDEs created");

					parse_file.println("***********************************************");
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": checking labels "); 

				} 	// verbose
			}	// eof reached
			break;
		case PARSE_LABEL:
			if (!check_label())
			{	// cheking labels done
				psw.program_data_element=0;
				psw.program_counter=0;
				if (verbose)
				{ 	// verbose
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": End checking labels ");
					parse_file.println("***********************************************");

					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": matching loops "); 

				} 	// verbose
				process_state=PARSE_LOOP_1;
			}	// cheking labels done
			break;
		case PARSE_LOOP_1:
			if (!match_loop())
			{	// match loop done
				psw.program_data_element=0;
				psw.program_counter=0;
				if (verbose)
				{ 	// verbose
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": End matching loops ");
					parse_file.println("***********************************************");

					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": checking loops "); 
				}	// verbose
				process_state=PARSE_LOOP_2;
			}	// match loop done
			break;
		case PARSE_LOOP_2:
			if (!check_loop())
			{	// match loop done
				psw.program_data_element=0;
				psw.program_counter=0;
				if (verbose)
				{ 	// verbose
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": End checking loops ");
					parse_file.println("***********************************************");

					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": message List "); 
				}	// verbose
				process_state=PARSE_MESSAGE;
			}	// match loop done
			break;
		case PARSE_MESSAGE:
			if (!list_message())
			{	// match loop done
				psw.program_data_element=0;
				psw.program_counter=0;
				if (verbose)
				{ 	// verbose
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.println(": End message list ");
					parse_file.println("***********************************************");
				}	// verbose
				process_state=CLOSE_FILE;
			}	// match loop done
			break;
		case CLOSE_FILE:
			program_file.close();
			error_file.close();
			log_file.close();
			if (verbose)
			{ 	// verbose
				parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
				parse_file.println(": done close file");
				parse_file.close();
			} 	// verbose

			psw.loaded=!psw.error;
			process_state=IDLE;
			break;
		case RUN:
			if (hasSD())
				process_program();
			else
				process_state=IDLE;
			break;
		case NO_CARD:
			break;
	}	// switch process_state
	
	return isBusy();
}	//	process

void program_c::startProgram(bool bStart, bool bAutoStart)
{	// start program
	if (is_loaded() && !has_errors())
	{	// ok prepare
		if (!is_running() || !bStart) // restarting not allowed, diable first
		{	//	start or stop program
			
			// reset psw
			psw.loop_level=0;
			psw.program_counter=0;
			psw.program_data_element=0;
			psw.msg_id=0;
			psw.msg_type=CLEAR_MSG;
			psw.delay=0;
			psw.last_call=millis();
			psw.current_time=psw.last_call;
			psw.loop_level=0;
			psw.auto_start=bAutoStart;
			
			for (uint8_t n=0; n<MaxFlagCnt; Flags[n++]=0);
			
			// output has to be controlled from within program
			// output will automaticalle be disabled on entry and exit
			pController->enable_output(false);
		
			if (is_running())
			{
				log_file.close();
				process_state=IDLE;
			}
			else if (bStart)
			{
				log_file=SD.open(log_file_name_ext,FILE_WRITE);
				log_header();
				process_state=RUN;
			}
		}	//	start or stop program
	}	// ok prepare
}	// start program


void program_c::getMessageState(message_type_et & msg_type, uint8_t & msg_id)
{	// getMessageState
	msg_type=psw.msg_type;
	msg_id=psw.msg_id;
}	// getMessageState

const char * program_c::getMessage(uint8_t msg_id)
{	// getMessage
	return get_list_text(Messages,msg_id);
}	// getMessage


/************************************************************
 * private
 * **********************************************************/
 
void program_c::clear_program_data(void)
{	// clear_program_data
	uint16_t cnt=sizeof(ProgramData);
	uint8_t *pData=(uint8_t*)ProgramData;

	while (cnt--) *pData++=NUL;
	
	for (cnt=0;cnt<MaxMessageBufferLength;cnt++) 
		Messages[cnt]=NUL;

	for (cnt=0;cnt<MaxFlagCnt;cnt++) 
		Messages[cnt]=0;


	psw.loop_level=0;
	psw.program_counter=0;
	psw.program_data_element=0;
	psw.program_lines=0;
	psw.program_data_length=0;
	psw.message_pointer=0;
	psw.delay=0;
	psw.last_call=0;
	psw.current_time=0;
	psw.loop_level=0;
	psw.msg_id=0;
	psw.msg_type=CLEAR_MSG;
	psw.error=false;
	psw.loaded=false;
	psw.auto_start=false;
}	// clear_program_data

/* **********************
 * **********************
 * ** PARSER FUNCITONS **
 * **********************
 * ********************** */

void program_c::open_program_file(void)
{	//	open_program_file
	program_file=SD.open(program_file_name_ext);
}	//	open_program_file

void program_c::close_porgram_file(void)
{	// close_porgram_file
	program_file.close();
}	// close_porgram_file

bool program_c::read_program_file_line(void)
{	// read_program_file_line
	uint8_t	 	 cnt=0;
	char 		 data=NUL;

	while (program_file.available() && (data!=LF) && (cnt<PrgLineLen))
	{	// read line
		data=program_file.read();
		prg_line[cnt]=data;
		if ((data==CR) || (data==LF)) 
		{
			prg_line[cnt]=NUL;
		}
		cnt++;
	}	// read line
	prg_line[cnt]=NUL;
	
	if (cnt>0)
	{	// data available
		psw.program_lines++;

		if (verbose)
		{	// verbose
			parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
			parse_file.print(": line #"); parse_file.print(psw.program_lines);
			parse_file.print(" ");
			parse_file.flush();
		} // verbose

		if ((cnt==PrgLineLen) && (data!=LF))
		{	// error line does not fit into buffer
			log_error(LINE_TO_LONG,psw.program_lines);
		}	// error line does not fit into buffer
		else
		{	// no error, continue
			parse_line();
		}	// no error, continue
	}	// data available

	return cnt>0;
}	// read_program_file_line

void program_c::parse_line(void)
{	//	parse_line
	char* pData=prg_line;
	pData=skip_space(pData);
	
	if (*pData)
	{	// real command, continue
		switch ((command_et)find_token(pData,CMD_TEXT))
		{	// switch command
			case SET:			
				parse_set(pData);
				break;
			case WAIT:
				parse_wait(pData);
				break;
			case LOOP:
				parse_loop(pData);
				break;
			case JUMP:
				parse_jump(pData);
				break;
			case LABEL:
				parse_label(pData);
				break;
			case COMMENT:
				// just ignore
				if (verbose)
				{	// verbose
					pData++;
					parse_file.print(": parsing COMMENT:");
					parse_file.println(pData);
					parse_file.flush();
				}	// verbose
				break;
			case LOG:
				parse_log(pData);
				break;
			case DISP:
				/* fall through */
			case INFO:
				/* fall through */
			case PASS:
				/* fall through */
			case FAIL:
				/* fall through */
			case CLEAR:
				parse_disp(pData);
				break;
			case LOAD:
				parse_load(pData);
				break;
			case SAVE:
				parse_save(pData);
				break;
			default:
				// error unknown command
				log_error(UNKNOWN_COMMAND,psw.program_lines);
				if (verbose)
				{	// verbose
					parse_file.print(": parsing UNKNOWN:");
					parse_file.println(pData);
					parse_file.flush();
				}	// verbose
				break;
		}	// switch command
	}	// real command, continue
	else
	{	// empty line
		if (verbose)
			parse_file.println(" - / - ");
	}	// empty line
}	//	parse_line

void program_c::parse_set(char * pData)
{	// parse_set
	const uint8_t max_id=4;
	uint8_t pos,op,id,n; 
	uint8_t	id_cnt[max_id]={0};	
	
	/* *********************
	 * ID: O:0, I:1, U:2, F:3	
	 * OP: +:0, =:1, -:2
	 * id_cnt:
	 * 		0: idle
	 *   	1: ID selected
	 * 		2: OP selected
	 * 		3: value assiged
	 * ********************** */ 

	pos=0;
	op=0;	
	
	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)SET)));
	pData=skip_space(pData);
	
	ProgramData[psw.program_data_element].base.cmd=SET;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;

	
	while (*pData)
	{	// eol not reached
		pos=find_char(upper(*pData),SET_CHARS);
		id=0;
		n=0;

		if (pos!=0xff)
		{	// char exists

			switch (SET_CHARS[pos])
			{	// switch Variable / Operator
				case 'F':
					id++;
					// check for digit
					// might overwrite existing entry, but error is generated later
					ProgramData[psw.program_data_element].data.set.flag_number=calc_flag_number(pData);
					/* fall through */
				case 'U':
					id++;
					/* fall through */
				case 'I':
					id++;
					/* fall through */
				case 'O':
					for (n=0; (n<max_id) &&  ((id_cnt[n]==0) || (id_cnt[n]==3))  ; n++);
					if (n<max_id) // error, assignement pending 	
						log_error(EXPRESSION_PENDING,psw.program_lines);
					else if (id_cnt[id]==0) // ok start assignement
						id_cnt[id]=1;
					else // error, assignement already done
						log_error(VARIABLE_MULTIPLE_USE,psw.program_lines);
					// start of assignment, reset operator
					op=0;
					break;
				case '-':
					op++;
					/* fall through */
				case '=':
					op++;
					/* fall through */
				case '+':
					for (n=0; (n<max_id) && (id_cnt[n]!=2); n++);
					if (n==max_id)	
					{	// no pending value, continue
						for (n=0; (n<max_id) &&  (id_cnt[n]!=1); n++);
						bool bAssignmet=op==1;
						switch (n)
						{	// select target
							case 3: // flag
								ProgramData[psw.program_data_element].data.set.flagChange=bAssignmet;
								ProgramData[psw.program_data_element].data.set.flagIncrement=!bAssignmet;
								break;
							case 2:	// voltage
								ProgramData[psw.program_data_element].data.set.voltageChange=bAssignmet;
								ProgramData[psw.program_data_element].data.set.voltageIncrement=!bAssignmet;
								break;
							case 1:	// current
								ProgramData[psw.program_data_element].data.set.currentChange=bAssignmet;
								ProgramData[psw.program_data_element].data.set.currentIncrement=!bAssignmet;
								break;
							case 0:	// output
								ProgramData[psw.program_data_element].data.set.outputChange=bAssignmet;
								if (!bAssignmet)	// output only direct assignment
									log_error(IMMEDEATE_ONLY,psw.program_lines);
								break;
							default:
								// no target selected
								log_error(NO_VARIABLE,psw.program_lines);
								break;
						}	// select target
						
						if (n<max_id) id_cnt[n]=2;
					}	// no pending value, continue
					else // value pending, multiple operators
						log_error(MULTIPLE_OPERATORS,psw.program_lines);
					break;
				default:
					log_error(INVALID_PARAMETER,psw.program_lines);
					break;
			}	// switch Variable / Operator
			pData++;
		}	// char exisis
		else
		{ 	// check for numerical value
			pData=skip_space(pData);

			if (is_digit(*pData))
			{	//	numerical value
				for (n=0; (n<max_id) &&  (id_cnt[n]!=2); n++);
				if (n<max_id)
				{	// pending id found
					switch (n)
					{	// add value
						case 3:	// flag
							ProgramData[psw.program_data_element].data.set.flag_val=
								convert_c(pData).getUnsigned(0);
							if (op==2) ProgramData[psw.program_data_element].data.set.flag_val=
											-ProgramData[psw.program_data_element].data.set.flag_val;
							break;
						case 2:	// voltage
							ProgramData[psw.program_data_element].data.set.voltage_val=
								convert_c(pData).getUnsigned(3);
							if (op==2) ProgramData[psw.program_data_element].data.set.voltage_val=
											-ProgramData[psw.program_data_element].data.set.voltage_val;
							break;
						case 1:	// current
							ProgramData[psw.program_data_element].data.set.current_val=
								convert_c(pData).getUnsigned(3);
							if (op==2) ProgramData[psw.program_data_element].data.set.current_val=
											-ProgramData[psw.program_data_element].data.set.current_val;
							break;
						case 0:	// output
							ProgramData[psw.program_data_element].data.set.outputState=
								convert_c(pData).getUnsigned(0)!=0;
							break;
						default:
							// excluded by if clause
							break;
					}	// add value
					id_cnt[n]=3;
				}	// pending id found
				else
				{	// no pendig id, check delay
					for (n=0; (n<max_id) && (id_cnt[n]!=3); n++);
					if (n<max_id)
					{	// value is delay
						if (convert_c(pData).getUnsigned(0)>MaxDelay)
							log_error(DELAY_TO_LONG,psw.program_lines);
							
						if (convert_c(pData).getUnsigned(0)==0) 
							log_error(INVALID_DELAY,psw.program_lines);

						ProgramData[psw.program_data_element].data.set.delay=
							convert_c(pData).getUnsigned(0);
					}	// value is delay
					else
					{	// no value completed other error 
						for (n=0; (n<max_id) && (id_cnt[n]!=1); n++);
						if (n<max_id) // ID pending no operator selected
							log_error(NO_OPERATOR,psw.program_lines);
						else // no active or finished assignement
							log_error(NO_VARIABLE,psw.program_lines);
					}	// no value completed other error
				}	// no pendig id, check delay

				pData=skip_non_space(pData);
			}	//	numerical value
			else if (find_char(upper(*pData),SET_CHARS)==0xFF)
			{	// invalid parameter
				log_error(INVALID_PARAMETER,psw.program_lines);
				while (*pData) pData++;
			}	// invalid parameter
		}	// // check for numerical value
		pData=skip_space(pData);
	}	// eol not reached

	if (verbose)
	{	// verbose
		parse_file.print(": parsing SET, PDE=");
		parse_file.print(psw.program_data_element);
		if (ProgramData[psw.program_data_element].data.set.voltageChange)
		{
			parse_file.print(", U=");
			parse_file.print(ProgramData[psw.program_data_element].data.set.voltage_val);
			parse_file.print("mV");
		}
		if (ProgramData[psw.program_data_element].data.set.voltageIncrement)
		{
			parse_file.print(", U=U");
			if (ProgramData[psw.program_data_element].data.set.voltage_val>=0)
				parse_file.print("+");
			parse_file.print(ProgramData[psw.program_data_element].data.set.voltage_val);
			parse_file.print("mV");
		}
		if (ProgramData[psw.program_data_element].data.set.currentChange)
		{
			parse_file.print(", I=");
			parse_file.print(ProgramData[psw.program_data_element].data.set.current_val);
			parse_file.print("mA");
		}
		if (ProgramData[psw.program_data_element].data.set.currentIncrement)
		{
			parse_file.print(", I=I");
			if (ProgramData[psw.program_data_element].data.set.current_val>=0)
				parse_file.print("+");
			parse_file.print(ProgramData[psw.program_data_element].data.set.current_val);
			parse_file.print("mA");
		}
		
		if (ProgramData[psw.program_data_element].data.set.outputChange)
		{
			parse_file.print(", O=");
			if (ProgramData[psw.program_data_element].data.set.outputState)
				parse_file.print("ON");
			else
				parse_file.print("OFF");
		}

		if (ProgramData[psw.program_data_element].data.set.flagChange)
		{
			parse_file.print(", F");
			parse_file.print(ProgramData[psw.program_data_element].data.set.flag_number);
			parse_file.print("=");
			parse_file.print(ProgramData[psw.program_data_element].data.set.flag_val);
		}
		if (ProgramData[psw.program_data_element].data.set.flagIncrement)
		{
			parse_file.print(", F");
			parse_file.print(ProgramData[psw.program_data_element].data.set.flag_number);
			parse_file.print("=");
			parse_file.print(", F");
			parse_file.print(ProgramData[psw.program_data_element].data.set.flag_number);
			if (ProgramData[psw.program_data_element].data.set.flag_val>=0)
				parse_file.print("+");
			parse_file.print(ProgramData[psw.program_data_element].data.set.flag_val);
		}

		parse_file.print(", Delay= ");
		parse_file.print(ProgramData[psw.program_data_element].data.set.delay);
		parse_file.println("ms");
		parse_file.flush();
	}	// verbose
	psw.program_data_element++;
	
}	// parse_set

void program_c::parse_wait(char * pData)
{	// parse_wait
	uint32_t delay;
	
	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)WAIT)));
	
	delay=convert_c(pData).getUnsigned(0);
	ProgramData[psw.program_data_element].base.cmd=WAIT;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	ProgramData[psw.program_data_element].data.wait.delay=delay;
	if (delay>MaxDelay) log_error(DELAY_TO_LONG,psw.program_lines);
	if (delay==0) log_error(INVALID_DELAY,psw.program_lines);
	
	if (verbose)
	{	// verbose
		parse_file.print(": parsing WAIT, PDE=");
		parse_file.print(psw.program_data_element);
		parse_file.print(" Delay= ");
		parse_file.print(ProgramData[psw.program_data_element].data.wait.delay);
		parse_file.println("ms");
	}	// verbose
	/* ********************************************** */

	psw.program_data_element++;

}	// parse_wait

void program_c::parse_loop(char * pData)
{	// parse_loop
	uint16_t cnt=0;

	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)WAIT)));
	pData=skip_space(pData);
	if (*pData) cnt=convert_c(pData).getUnsigned(0);
	ProgramData[psw.program_data_element].base.cmd=LOOP;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	if (cnt>0)
	{	// argument given, loop header
		psw.loop_level++;
	}	// argument given, loop header

	ProgramData[psw.program_data_element].data.loop.Counter=cnt;
	ProgramData[psw.program_data_element].data.loop.level=psw.loop_level;
	ProgramData[psw.program_data_element].data.loop.TargetPDE=0;

	if (cnt==0)
	{	// loop footer
		if (psw.loop_level>0) 
			psw.loop_level--;
		else 
			log_error(NO_OPEN_LOOP,psw.program_lines);
		
	}	// loop footer

	if (verbose)
	{ 	// verbose
		parse_file.print(": parsing LOOP, PDE=");
		parse_file.print(psw.program_data_element);
		if (cnt>0)
		{	// open loop
			parse_file.print(", open Level ");
			parse_file.print(ProgramData[psw.program_data_element].data.loop.level);
			parse_file.print(", for ");
			parse_file.print(ProgramData[psw.program_data_element].data.loop.Counter);
			parse_file.print(" cycles in line #");
			parse_file.println(ProgramData[psw.program_data_element].base.line);
		}	// open loop
		else
		{	// close loop
			parse_file.print(", close Level ");
			parse_file.print(ProgramData[psw.program_data_element].data.loop.level);
			parse_file.print(" in line #");
			parse_file.println(ProgramData[psw.program_data_element].base.line);
		}	// close loop
	} 	// verbose

	psw.program_data_element++;
}	// parse_loop

void program_c::parse_jump(char * pData)
{	// parse_jump
	uint8_t pos,cnt;

	pos=0;
	cnt=0;
	
	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)JUMP)));
	pData=skip_space(pData);
	
	ProgramData[psw.program_data_element].base.cmd=JUMP;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	ProgramData[psw.program_data_element].data.jump.source=(uint8_t)JUMP_UNCONDITIONAL;
	
	while (*pData)
	{	// eol not reached
		pos=find_char(upper(*pData),JUMP_CHARS);

		if (pos!=0xff)
		{	// char exists
			switch (JUMP_CHARS[pos])
			{	// switch Variable / Operator
				case 'F':
					/* fall through */
				case 'U':
					/* fall through */
				case 'I':
					/* fall through */
				case 'O':
					/* fall through */
				case 'R':
					if ((jump_src_et)ProgramData[psw.program_data_element].data.jump.source==JUMP_UNCONDITIONAL)
					{	// condition not yet set
						ProgramData[psw.program_data_element].data.jump.source=(uint8_t)calc_jump_source(pData);
						if (ProgramData[psw.program_data_element].data.jump.source==JUMP_ON_FLAG_COMPARE)
							ProgramData[psw.program_data_element].data.jump.flag_number=calc_flag_number(pData);
						cnt=1;
					}	// condition not yet set
					else
					{	// condition allready set
						log_error(MULTIPLE_CONDITIONS,psw.program_lines);
					}	// condition allready set
					break;
				case '>':
					/* fall through */
				case '<':
					/* fall through */
				case '=':
					if (cnt==0)
					{	// error, no variale selected
						log_error(NO_VARIABLE,psw.program_lines);
					}	// error, no variale selected
					else if (cnt==2)
					{	// error, multiple operators
						log_error(MULTIPLE_OPERATORS,psw.program_lines);
					}	// error, multiple operators
					else
					{	// allowed ?
						if ( (JUMP_CHARS[pos]=='=') && 
							 ( ((jump_src_et)ProgramData[psw.program_data_element].data.jump.source==JUMP_ON_CURRENT_COMPARE) ||
							   ((jump_src_et)ProgramData[psw.program_data_element].data.jump.source==JUMP_ON_VOLTAGE_COMPARE) ) )
						{	// illegal operator
							log_error(INVALID_OPERATOR,psw.program_lines);
						}	// illegal operator
						ProgramData[psw.program_data_element].data.jump.greater_than=JUMP_CHARS[pos]=='>';
						ProgramData[psw.program_data_element].data.jump.less_than=JUMP_CHARS[pos]=='<';
						cnt=2;
					}	// allowed ?
					break;
				case ':':
					if ((cnt!=0) && (cnt!=3))
					{	// expression not completed
						log_error(EXPRESSION_PENDING,psw.program_lines);
					}	// expression not completed
					pData++;
					ProgramData[psw.program_data_element].data.jump.label_code=calc_label(pData);
					pData=skip_non_space(pData);
					pData--;
					break;
				default:
					log_error(INVALID_PARAMETER,psw.program_lines);
					break;
			}	// switch Variable / Operator
			pData++;
		}	// char exisis
		else
		{ 	// check for numerical value
			if (is_digit(*pData) || is_sign(*pData))
			{	//	numerical value
				if (cnt==2)
				{	// extract value
					switch (ProgramData[psw.program_data_element].data.jump.source)
					{	// switch source
						case JUMP_ON_CURRENT_COMPARE:
							/* fall through */
						case JUMP_ON_VOLTAGE_COMPARE:
							ProgramData[psw.program_data_element].data.jump.Reference=
								convert_c(pData).getSigned(3);
							break;
						case JUMP_ON_FLAG_COMPARE:
							/* fall through */
						case JUMP_ON_AUTO_START_COMPARE:
							/* fall through */
						case JUMP_ON_OUTPUT_COMPARE:
							ProgramData[psw.program_data_element].data.jump.Reference=
								convert_c(pData).getUnsigned(0);
							break;
						default:
							break;
					}	// switch source
					cnt=3;
				}	// extract value
				else if (cnt==0)
				{	// no variable
					log_error(NO_VARIABLE,psw.program_lines);
				}	// no variable
				else
				{	// no operator
					log_error(NO_OPERATOR,psw.program_lines);
				}	// no operator
				pData=skip_non_space(pData);
			}	//	numerical value
			else if (find_char(upper(*pData),JUMP_CHARS)==0xFF)
			{	// invalid parameter
				log_error(INVALID_PARAMETER,psw.program_lines);
				while (*pData) pData++;
			}	// invalid parameter
		}	// // check for numerical value
		pData=skip_space(pData);
	}	// eol not reached

	if (verbose)
	{ 	// verbose
		parse_file.print(": parsing JUMP, PDE=");
		parse_file.print(psw.program_data_element);
		
		switch ((jump_src_et)ProgramData[psw.program_data_element].data.jump.source)
		{	// switch relation source
			case JUMP_ON_VOLTAGE_COMPARE:
				parse_file.print(" JUMP on U");
				break;
			case JUMP_ON_CURRENT_COMPARE:
				parse_file.print(" JUMP on I");
				break;
			case JUMP_ON_OUTPUT_COMPARE:
				parse_file.print(" JUMP on O");
				break;
			case JUMP_ON_AUTO_START_COMPARE:
				parse_file.print(" JUMP on R");
				break;
			case JUMP_ON_FLAG_COMPARE:
				parse_file.print(" JUMP on F");
				parse_file.print(ProgramData[psw.program_data_element].data.jump.flag_number);
				break;
			default:
				parse_file. print(" JUMP unconditional");
				break;
		}	//  switch relation source
		
		if (ProgramData[psw.program_data_element].data.jump.source!=JUMP_UNCONDITIONAL)
		{
			if (ProgramData[psw.program_data_element].data.jump.greater_than)
				parse_file.print(" > ");
			else if (ProgramData[psw.program_data_element].data.jump.less_than)
				parse_file.print(" < ");
			else
				parse_file.print(" = ");
				
			parse_file.print(ProgramData[psw.program_data_element].data.jump.Reference);
			
			switch (ProgramData[psw.program_data_element].data.jump.source)
			{	// switch relation source
				case JUMP_ON_VOLTAGE_COMPARE:
					parse_file. print(" mV");
					break;
				case JUMP_ON_CURRENT_COMPARE:
					parse_file. print(" mA");
					break;
				case JUMP_ON_OUTPUT_COMPARE:
					/* fall through */
				case JUMP_ON_AUTO_START_COMPARE:
					/* fall through */
				case JUMP_ON_FLAG_COMPARE:
					/* fall through */
				default:
					break;
			}	//  switch relation source
		}
		
		parse_file.print(" LabelID= ");
		parse_file.println(ProgramData[psw.program_data_element].data.label.label_code,HEX);
	} 	// verbose

	psw.program_data_element++;
	
}	// parse_jump

void program_c::parse_label(char * pData)
{	// parse_label
	pData++;	// Label is only one char
	ProgramData[psw.program_data_element].base.cmd=LABEL;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	ProgramData[psw.program_data_element].data.label.label_code=calc_label(pData);

	if (verbose)
	{ 	// verbose
		parse_file.print(": parsing LABEL, PDE=");
		parse_file.print(psw.program_data_element);
		parse_file.print(", LabelID= ");
		parse_file.println(ProgramData[psw.program_data_element].data.label.label_code,HEX);
	} 	// verbose
	psw.program_data_element++;

}	// parse_label

void program_c::parse_log(char * pData)
{	// parse_log
	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)LOG)));
	pData=skip_space(pData);

	ProgramData[psw.program_data_element].base.cmd=LOG;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	ProgramData[psw.program_data_element].data.log.msg_id=-1;

	if (*pData)
	{	// log info given
		if (get_length(pData)+psw.message_pointer<MaxMessageBufferLength)
		{	// text fits into buffer
			ProgramData[psw.program_data_element].data.log.msg_id=
				add_list_text(Messages,pData);
		}	// text fits into buffer
		else
			log_error(OUT_OF_MEMORY,psw.program_lines);
	}	// log info given

	if (verbose)
	{ 	// verbose
		parse_file.print(": parsing LOG, PDE=");
		parse_file.print(psw.program_data_element);
		parse_file.print(", MessageID= ");
		parse_file.print(ProgramData[psw.program_data_element].data.log.msg_id);
		parse_file.print(" ::");
		parse_file.print(get_list_text(Messages,ProgramData[psw.program_data_element].data.log.msg_id));
		parse_file.println("::");
	} 	// verbose
	psw.program_data_element++;
}	// parse_log

void program_c::parse_disp(char * pData)
{	// parse_disp

	ProgramData[psw.program_data_element].base.cmd=DISP;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	ProgramData[psw.program_data_element].data.disp.msg_id=-1;

	switch ((command_et)find_token(pData,CMD_TEXT))
	{	// switch command
		case DISP:
			pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)DISP)));
			ProgramData[psw.program_data_element].data.disp.msg_type=DISP_MSG;			
			break;
		case INFO:
			pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)INFO)));
			ProgramData[psw.program_data_element].data.disp.msg_type=INFO_MSG;			
			break;
		case PASS:
			pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)PASS)));
			ProgramData[psw.program_data_element].data.disp.msg_type=PASS_MSG;			
			break;
		case FAIL:
			pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)FAIL)));
			ProgramData[psw.program_data_element].data.disp.msg_type=FAIL_MSG;			
			break;
		case CLEAR:
			pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)CLEAR)));
			/* fall through */
		default:
			ProgramData[psw.program_data_element].data.disp.msg_type=CLEAR_MSG;			
			break;
	}	// switch command
	
	pData=skip_space(pData);

	if (*pData)
	{	// disp info given
		if (get_length(pData)+psw.message_pointer<MaxMessageBufferLength-3)
		{	// text fits into buffer
			ProgramData[psw.program_data_element].data.disp.msg_id=
				add_list_text(Messages,pData);
			if (ProgramData[psw.program_data_element].data.disp.msg_type==CLEAR_MSG)
			{	// error unexpected data
				log_error(EXTRA_DATA,ProgramData[psw.program_data_element].base.line);
			}	// error unexpected data
		}	// text fits into buffer
		else
			log_error(OUT_OF_MEMORY,psw.program_lines);
	}	// disp info given
	else if (ProgramData[psw.program_data_element].data.disp.msg_type!=CLEAR_MSG)
	{	// disp info expected
		log_error(NO_DATA,psw.program_lines);
	}	// disp info expected
	
	if (verbose)
	{ 	// verbose
		parse_file.print(": parsing disp, PDE=");
		parse_file.print(psw.program_data_element);
		parse_file.print(", MessageType=");
		switch (ProgramData[psw.program_data_element].data.disp.msg_type)
		{	// switch message type
			case DISP_MSG:
				parse_file.print("DISP");
				break;
			case INFO_MSG:
				parse_file.print("INFO");
				break;
			case PASS_MSG:
				parse_file.print("PASS");
				break;
			case FAIL_MSG:
				parse_file.print("FAIL");
				break;
			case CLEAR_MSG:
				parse_file.print("CLEAR");
				break;
			default:
				parse_file.print("- / -");
				break;
		}	// switch message type
		parse_file.print(", MessageID= ");
		parse_file.print(ProgramData[psw.program_data_element].data.disp.msg_id);
		parse_file.print(" ");
		parse_file.println(get_list_text(Messages,ProgramData[psw.program_data_element].data.disp.msg_id));
	} 	// verbose

	psw.program_data_element++;
}	// parse_disp

void program_c::parse_load(char * pData)
{	// parse_load
	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)LOAD)));
	pData=skip_space(pData);

	ProgramData[psw.program_data_element].base.cmd=LOAD;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	
	if (*pData)
			log_error(EXTRA_DATA,psw.program_lines);

	if (verbose)
	{ 	// verbose
		parse_file.print(": parsing load, PDE=");
		parse_file.println(psw.program_data_element);
	} 	// verbose
	
	psw.program_data_element++;
}	// parse_load

void program_c::parse_save(char * pData)
{	// parse_save
	pData=skip_n(pData,get_length(get_list_text(CMD_TEXT,(uint8_t)SAVE)));
	pData=skip_space(pData);

	ProgramData[psw.program_data_element].base.cmd=SAVE;
	ProgramData[psw.program_data_element].base.line=psw.program_lines;
	
	if (*pData)
			log_error(EXTRA_DATA,psw.program_lines);

	if (verbose)
	{ // verbose
		parse_file.print(": parsing save, PDE=");
		parse_file.println(psw.program_data_element);
	} // verbose

	psw.program_data_element++;
}	// parse_save


bool program_c::check_label(void)
{	// check duplicate / missing label
	bool is_label,is_jump,match;
	
	is_label=false;
	is_jump=false;
	match=false;
	
	/* *******************************************************
	 * Check for duplicate label:
	 * scan program from beginning for label command
	 * when found, search from that line to end for equal labels
	 * if no equal label found check pass, proceed with next label command
	 * otherwise duplicate label found
	 * 
	 * Check for missing label:
	 * scan program from beginning for jump command
	 * search program from beginning for equal label 
	 * when label command with equal id is found proceed with next jump
	 * when no match has been found the target label is undefined
	 */

	while ( (psw.program_counter<psw.program_data_length) &&
			!is_label && !is_jump)
	{ 	// search for label or jump 
		is_label=(ProgramData[psw.program_counter].base.cmd==LABEL);
		is_jump= (ProgramData[psw.program_counter].base.cmd==JUMP);
		if (is_label || is_jump)
		{	// scan for labels
			if (verbose)
			{	// print verbose data
				if (is_label)
				{	// print label
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.print(": label in line "); 
					parse_file.print(ProgramData[psw.program_counter].base.line);
					parse_file.print(", PDE="); 
					parse_file.print(psw.program_counter);
					parse_file.print(", label id="); 
					parse_file.println(ProgramData[psw.program_counter].data.label.label_code,HEX);
				}	// print label
			}	// print verbose data
			
			// start on current label to prevent mulitple errors
			for (psw.program_data_element=(is_jump)?(0):(psw.program_counter); 
				 (psw.program_data_element<psw.program_data_length) && !match;
				 psw.program_data_element++)
			{	// scan pdes

				if (ProgramData[psw.program_data_element].base.cmd==LABEL)
				{	// LABEL cmd found, check match
					if (is_label)
						match=	(ProgramData[psw.program_counter].data.label.label_code == 
									ProgramData[psw.program_data_element].data.label.label_code) &&
								(psw.program_counter!=psw.program_data_element);
					if (is_jump)
						match=	(ProgramData[psw.program_counter].data.jump.label_code == 
									ProgramData[psw.program_data_element].data.label.label_code);
				}	// LABEL cmd found, check match
				
				
				if (match)
				{	// match, check if error
					if (is_label)
					{	// duplicate label found
						log_error(DUPLICATE_LABEL,ProgramData[psw.program_data_element].base.line);
					}	// duplicate label found
					
					if (is_jump)
					{	// success: jump target found
						ProgramData[psw.program_counter].data.jump.TargetPDE = 
							psw.program_data_element;
						if (verbose)
						{	// verbose output
							parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
							parse_file.print(": JUMP in line "); 
							parse_file.print(ProgramData[psw.program_counter].base.line);
							parse_file.print(", PDE="); 
							parse_file.print(psw.program_counter);
							parse_file.print(", label id="); 
							parse_file.print(ProgramData[psw.program_counter].data.label.label_code,HEX);
							parse_file.print(", Target: PDE="); 
							parse_file.print(psw.program_data_element);
							parse_file.print(", line "); 
							parse_file.println(ProgramData[psw.program_data_element].base.line);
						}	// verbose output
					}	// success: jump target found
				}	// match, check if error
			}	// scan pdes
			
			if (is_jump && !match)
			{	// jump and no match => error
				log_error(UNDEFINED_LABEL,ProgramData[psw.program_counter].base.line);
				if (verbose)
				{	// verbose output
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.print(": JUMP in line "); 
					parse_file.print(ProgramData[psw.program_counter].base.line);
					parse_file.print(", PDE="); 
					parse_file.print(psw.program_counter);
					parse_file.print(", label id="); 
					parse_file.print(ProgramData[psw.program_counter].data.label.label_code,HEX);
					parse_file.println(", Target: -/-"); 
				}	// verbose output
			}	// jump and no match => error

		}	// scan for labels
		psw.program_counter++;
	}	// search for label or jump
	
	return psw.program_counter<psw.program_data_length;
}	// check duplicate / missing label

bool program_c::match_loop(void)
{	// check loop
	bool is_loop,match;
	
	/* *********************************************************
	 * Assign loop targets
	 * scan program from beginning for closeing loop commands,
	 * when found scan backwords for opening loop with equal level
	 * when found assign footer and header PDE
	 */

	is_loop=false;
	match=false;
	
	while ( (psw.program_counter<psw.program_data_length) && !is_loop)
	{ 	// search closing loop
		is_loop=(ProgramData[psw.program_counter].base.cmd==LOOP) &&	// is loop
				(ProgramData[psw.program_counter].data.loop.Counter==0);// is cloing loop
		
		if (is_loop)
		{	// ok, loop footer found
			psw.program_data_element=psw.program_counter;
			while ((psw.program_data_element>0) && !match)
			{	// search for matching loop header 
				psw.program_data_element--;
				match=
					(ProgramData[psw.program_data_element].base.cmd==LOOP) &&			// is loop
					(ProgramData[psw.program_data_element].data.loop.Counter>0) &&		// is header		
					(ProgramData[psw.program_counter].data.loop.level==				// same level
						ProgramData[psw.program_data_element].data.loop.level) &&	
					(ProgramData[psw.program_data_element].data.loop.TargetPDE==0);		// not assigned
			}	// search for matching loop header

			if (match)
			{	//	matching loop header found 
				ProgramData[psw.program_counter].data.loop.TargetPDE=psw.program_data_element+1;
				ProgramData[psw.program_data_element].data.loop.TargetPDE=psw.program_counter;
				if (verbose)
				{	// verbose
					parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
					parse_file.print(": LOOP header line "); 
					parse_file.print(ProgramData[psw.program_data_element].base.line);
					parse_file.print(", PDE="); 
					parse_file.print(psw.program_data_element);
					parse_file.print(", level=");
					parse_file.print(ProgramData[psw.program_data_element].data.loop.level);
					parse_file.print(", cycles=");
					parse_file.print(ProgramData[psw.program_data_element].data.loop.Counter);
					parse_file.print(" ==> footer line "); 
					parse_file.print(ProgramData[psw.program_counter].base.line);
					parse_file.print(", PDE="); 
					parse_file.println(psw.program_counter);
				}	// verbose
			}	//	matching loop header found
			else
			{	// error no matching loop header found
				// error has allreade been reported in first pass
				// log_error(NO_OPEN_LOOP,ProgramData[psw.program_counter].base.line);
				parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
				parse_file.print(": LOOP  header not found"); 
				parse_file.print(" ==> footer line "); 
				parse_file.print(ProgramData[psw.program_counter].base.line);
				parse_file.print(", PDE="); 
				parse_file.print(psw.program_counter);
				parse_file.print(", level=");
				parse_file.println(ProgramData[psw.program_data_element].data.loop.level);
			}	// error no matching loop header found
		}	// ok, loop footer found
		psw.program_counter++;
	}	// search closing loop

	return psw.program_counter<psw.program_data_length;
}	// check loop

bool program_c::check_loop(void)
{	//	check_loop
	bool is_loop,error;
	/* *********************************************************
	 * scan program from beginning for unassinged loop commands,
	 */
	
	is_loop=false;
	error=false;
	
	while ( (psw.program_counter<psw.program_data_length) && !is_loop)
	{ 	// search loop
		is_loop=(ProgramData[psw.program_counter].base.cmd==LOOP);
		if (is_loop)
		{	// loop cmd found
			error=ProgramData[psw.program_counter].data.loop.TargetPDE==0;
			if (error)
			{	// unassigned loop found
				if (ProgramData[psw.program_counter].data.loop.Counter>0)
				{	// is header
					log_error(NO_CLOSE_LOOP,ProgramData[psw.program_counter].base.line);
					if (verbose)
					{	// verbose
						parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
						parse_file.print(": LOOP header unassigned line "); 
						parse_file.print(ProgramData[psw.program_counter].base.line);
						parse_file.print(", PDE="); 
						parse_file.print(psw.program_counter);
						parse_file.print(", level=");
						parse_file.print(ProgramData[psw.program_counter].data.loop.level);
						parse_file.print(", cycles=");
						parse_file.println(ProgramData[psw.program_counter].data.loop.Counter);
					}	// verbose
				}	// is header
				else
				{ 	// is footer
					// loop footer errors has been reported in first pass
					if (verbose)
					{	// verbose
						parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
						parse_file.print(": LOOP footer unassigned line "); 
						parse_file.print(ProgramData[psw.program_counter].base.line);
						parse_file.print(", PDE="); 
						parse_file.print(psw.program_counter);
						parse_file.print(", level=");
						parse_file.print(ProgramData[psw.program_counter].data.loop.level);
						parse_file.print(", cycles=");
						parse_file.println(ProgramData[psw.program_counter].data.loop.Counter);
					}	// verbose
				}	// is footer
			}	// unassigned loop found
			is_loop=is_loop && error;
		}	// loop cmd found
		psw.program_counter++;
	}	// search loop

	return psw.program_counter<psw.program_data_length;
	
}	//	check_loop

bool program_c::list_message(void)
{	// list messages
	if (*get_list_text(Messages,psw.program_counter))
	{	// message exisits
		if (verbose)
		{	// verbose
			parse_file.print(convert_c(millis(),0).getStringUnsigned(buffer,6,0));
			parse_file.print(": Message #"); 
			parse_file.print(psw.program_counter);
			parse_file.print(", length=");
			parse_file.print(get_length(get_list_text(Messages,psw.program_counter)));
			parse_file.print(" ::");
			parse_file.print(get_list_text(Messages,psw.program_counter));
			parse_file.println("::");
		}	// verbose
		psw.program_counter++;
	}	// message exisits
	
	return *get_list_text(Messages,psw.program_counter);
}	// list messages


program_c::jump_src_et program_c::calc_jump_source(char * source)
{	// calc
	jump_src_et result;
	
	switch (upper(*source))
	{	// find source
		case 'U':
			result=JUMP_ON_VOLTAGE_COMPARE;
			break;
		case 'I':
			result=JUMP_ON_CURRENT_COMPARE;
			break;
		case 'O':
			result=JUMP_ON_OUTPUT_COMPARE;
			break;
		case 'R':
			result=JUMP_ON_AUTO_START_COMPARE;
			break;
		case 'F':
			result=JUMP_ON_FLAG_COMPARE;
			break;
		default:
			result=JUMP_UNCONDITIONAL;
			break;
	}	// find source
	
	return result;
}

uint32_t program_c::calc_label(char * pData)
{	// calc_label
	uint8_t 	cnt=LabelLength;
	uint32_t	label=0;
	
	if (find_char(*pData,LABEL_CHARS)!=0xFF)
	{	// first char ok
		uint8_t val;
		bool	is_label;
		do
		{	// claculate label val
			cnt--;
			val=find_char(*pData,LABEL_CHARS);
			is_label=(uint8_t)(~val)!=0;
			if (is_label)
			{
				label=(label<<6)+val;
				pData++;
			}
		}	// claculate label val
		while ((cnt) && (is_label));
		
		if (*pData)
		{	// calculation finished and still charcters
			if (cnt==0)
			{	// still characters -> label to long
				log_error(LABEL_TO_LONG,psw.program_lines);
			}	// still characters -> label to long
			else if ((!is_label) && (!is_whitespace(*pData)))
			{	// non whitespace in sequence and no Label char
				log_error(INVALID_LABEL_CHAR,psw.program_lines);
			}	// non whitespace in sequence and no Label char
		}	// calculation finished and still charcters
		
	}	// first char ok
	else
	{ 	// invalid label char
		log_error(INVALID_LABEL_CHAR,psw.program_lines);
	}	// invalid label char
	return label;
}	// calc_label


uint8_t program_c::calc_flag_number(char * &source)
{	//	calc_flag_number
	uint8_t number=0;
	
	source++;
	if (is_digit(*source))
	{	// ok flag has been selected
		number=*source-'0';
	}	// ok flag has been selected
	else
	{	// missing index
		source--;
		log_error(NO_INDEX,psw.program_lines);
	}	// missing index

	return number;
}	// calc_flag_number


void program_c::log_error(error_et error, uint16_t line)
{	// log Error
	char tmp[4];
	error_file=SD.open(error_file_name_ext,FILE_WRITE);
	error_file.print(convert_c(line,0).getStringUnsigned(tmp,3,0));
	error_file.print(": ");
	error_file.println(get_list_text(ERROR_TEXT,(uint8_t)error));
	error_file.close();
	psw.error=true;
}	// log Error

/* ***********************************
 * ***********************************
 * ** program processing functions  **
 * ***********************************
 * *********************************** */

void program_c::process_program(void)
{	//	process_program
	psw.current_time=millis();
	if (psw.delay)
	{	// delay active
		if (psw.current_time-psw.last_call>psw.delay)
			psw.delay=0;
		else
			psw.delay-=(psw.current_time-psw.last_call);
	}	// delay active
	else 
	{	// no delay proceed
		if (psw.program_counter<psw.program_data_length)
		{	// still pdes to process
			switch (ProgramData[psw.program_counter].base.cmd)
			{	// switch command
				case SET:
					process_set();
					break;
				case WAIT:
					process_wait();
					break;
				case LOOP:
					process_loop();
					break;
				case JUMP:
					process_jump();
					break;
				case LABEL:
					psw.program_counter++;
					break;
				case LOG:	
					process_log();
					break;
				case DISP:	
					/* fall through */
				case INFO:	
					/* fall through */
				case PASS:	
					/* fall through */
				case FAIL:	
					/* fall through */
				case CLEAR:
					process_disp();
					break;
				case LOAD:	
					process_load();
					break;
				case SAVE:	
					process_save();
					break;
				case COMMENT:
					/* fall through */
				default:
					process_error();
					break;
			}	// switch command
		}	// still pdes to process
		else
		{	// program finished
			// turn off output and close log
			log_file.print("m;    ;");
			log_time();
			log_file.println("\"program terminated\"");
			
			log_file.close();
			pController->enable_output(false);
			process_state=IDLE;
	
			// clear last message on program termination
			psw.msg_type=CLEAR_MSG;
		}	// program finished
	}	// no delay proceed
	
	psw.last_call=psw.current_time;
}	//	process_program

void program_c::process_set  (void)
{	// process_set
	int16_t u,i;
	bool current,voltage;
	
	current=ProgramData[psw.program_counter].data.set.currentChange ||
			ProgramData[psw.program_counter].data.set.currentIncrement;
	voltage=ProgramData[psw.program_counter].data.set.voltageChange ||
	        ProgramData[psw.program_counter].data.set.voltageIncrement;
	
	if (ProgramData[psw.program_counter].data.set.currentChange)
		i=ProgramData[psw.program_counter].data.set.current_val;
	else if (ProgramData[psw.program_counter].data.set.currentIncrement)
		i=pController->get_set_current()+
			ProgramData[psw.program_counter].data.set.current_val;
	else
		i=pController->get_set_current();

	if (ProgramData[psw.program_counter].data.set.voltageChange)
		u=ProgramData[psw.program_counter].data.set.voltage_val;
	else if (ProgramData[psw.program_counter].data.set.voltageIncrement)
		u=pController->get_set_voltage()+
			ProgramData[psw.program_counter].data.set.voltage_val;
	else
		u=pController->get_set_current();

	if (i<10) i=10;
	if (u<pController->get_min_voltage_mV()) u=pController->get_min_voltage_mV();
	if (u>pController->get_max_voltage_mV(0)) u=pController->get_max_voltage_mV(0);
	if (i>pController->get_max_current_mA(u)) i=pController->get_max_current_mA(u);
	
	if (current || voltage)
		pController->set_power(u,i);
	
	if (ProgramData[psw.program_counter].data.set.flagChange)
		Flags[ProgramData[psw.program_counter].data.set.flag_number]=
			ProgramData[psw.program_counter].data.set.flag_val;
	else if (ProgramData[psw.program_counter].data.set.flagIncrement)
		Flags[ProgramData[psw.program_counter].data.set.flag_number]+=
			ProgramData[psw.program_counter].data.set.flag_val;
	
	if (ProgramData[psw.program_counter].data.set.outputChange)
		pController->enable_output(ProgramData[psw.program_counter].data.set.outputState);
		
	psw.delay=ProgramData[psw.program_counter].data.set.delay;
	psw.program_counter++;
}	// process_set

void program_c::process_wait (void)
{	// 	process_wait
	psw.delay=ProgramData[psw.program_counter].data.wait.delay;
	psw.program_counter++;
}	//  process_wait

void program_c::process_loop (void)
{	// process_loop
	
	if (psw.program_counter<ProgramData[psw.program_counter].data.loop.TargetPDE)
	{	// loop statement is header
		// set loop counter
		ProgramData[ProgramData[psw.program_counter].data.loop.TargetPDE].data.loop.Counter=
			ProgramData[psw.program_counter].data.loop.Counter;
			psw.program_counter++;
	}	// loop statement is header
	else
	{	// loop statement is footer
		// decrement loop Counter
		if (ProgramData[psw.program_counter].data.loop.Counter>0)
			ProgramData[psw.program_counter].data.loop.Counter--;
		// jump back if nonzero
		if (ProgramData[psw.program_counter].data.loop.Counter>0)
			psw.program_counter=ProgramData[psw.program_counter].data.loop.TargetPDE;
		else
			psw.program_counter++;
	}	// loop statement is footer
}	// process_loop

void program_c::process_jump (void)
{	// process_jump
	bool jump;
	int16_t val;
	
	jump=false;
	
	switch (ProgramData[psw.program_counter].data.jump.source)
	{	// switch source
		case JUMP_ON_VOLTAGE_COMPARE:
			val=pController->get_output_voltage_mV();
			break;
		case JUMP_ON_CURRENT_COMPARE:
			val=pController->get_output_current_mA();
			break;
		case JUMP_ON_OUTPUT_COMPARE:
			val=(pController->is_output_enabled())?(1):(0);
			break;
		case JUMP_ON_AUTO_START_COMPARE:
			val=(psw.auto_start)?(1):(0);
			break;
		case JUMP_ON_FLAG_COMPARE:
			val=Flags[ProgramData[psw.program_counter].data.jump.flag_number];
			break;
		case JUMP_UNCONDITIONAL:
			/* fall through */
		default:
			jump=true;
			break;
	}	// switch source

	jump=jump ||
		(	(val>ProgramData[psw.program_counter].data.jump.Reference) && 
			ProgramData[psw.program_counter].data.jump.greater_than	) ||
		(	(val<ProgramData[psw.program_counter].data.jump.Reference) && 
			ProgramData[psw.program_counter].data.jump.less_than	) ||
		(	(ProgramData[psw.program_counter].data.jump.Reference==val) && 
			!ProgramData[psw.program_counter].data.jump.greater_than &&
			!ProgramData[psw.program_counter].data.jump.less_than	);

	if (jump)
		psw.program_counter=ProgramData[psw.program_counter].data.jump.TargetPDE;
	else
		psw.program_counter++;
}	// process_jump

void program_c::process_log  (void)
{	// process_log
	if ((uint8_t)(~ProgramData[psw.program_counter].data.log.msg_id))
		log_message();
	else
		log_data();
	psw.program_counter++;
}	// process_log

void program_c::process_disp (void)
{	// process_disp
	psw.msg_id=ProgramData[psw.program_counter].data.disp.msg_id;
	psw.msg_type=ProgramData[psw.program_counter].data.disp.msg_type;
	psw.program_counter++;
}	// process_disp

void program_c::process_load (void)
{	// process_load
	// load flags from eeprom / sd card
	pParameter->getFlags(Flags);
	psw.program_counter++;
}	// process_load

void program_c::process_save (void)
{	// process_save
	// store flages to eeprom / sd card
	pParameter->setFlags(Flags);
	pParameter->write();
	psw.program_counter++;
}	// process_save

void program_c::process_error(void)
{	// process error
	// an error has occured
	log_file.print("e;");
	log_line();
	log_time();
	log_file.println("\"runtime error\"");
	
	// set psw to terminat
	psw.error=true;
	psw.delay=0;
	psw.program_counter=psw.program_data_length;
}	// process error


/* **********************
 * **********************
 * **  log  funtions   **
 * **********************
 * ********************** */

void program_c::log_header(void)
{	// log header
	log_file.println("****************************************************");
	log_file.println("* t;line;data    (t)ype: d=data, m=message, e=error");
	log_file.println("m;line;message");
	log_file.println("e;line;message");
	log_file.println("d;line;---- time -----;-U/V-;-I/A-;Uout/V;Iout/A;deg C");
	log_file.print("m;    ;");
	log_time();
	log_file.println("\"program started\"");
	log_file.flush();
}	// log header

void program_c::log_message(void)
{	// log_message
	log_file.print("m;");
	log_line();
	log_time();
	log_file.print("\"");
	log_file.print(get_list_text(Messages,ProgramData[psw.program_counter].data.log.msg_id));
	log_file.println("\"");
	log_file.flush();
}	// log_message

void program_c::log_data(void)
{	// log_data
	log_file.print("d;");
	log_line();
	log_time();
	
	// print data id
	
	/* *********************************************	
	 * currently no mode entry
	// log mode
	if (pController->is_PPS())
	{	// PPS
		switch (pController->get_operating_mode())
		{	// switch operating mode
			case controller_c::CONTROLLER_MODE_CV:
				log_file.print("U  ;");
				break;
			case controller_c::CONTROLLER_MODE_CVCC:
				log_file.print("UI ;");
				break;
			case controller_c::CONTROLLER_MODE_CVCCmax:
				log_file.print("UI^;");
				break;
			case controller_c::CONTROLLER_MODE_OFF:
			default:
				log_file.print("PPS;");
				break;
		}	// switch operating mode
	}	// PPS
	else
	{	// FIX
		log_file.print("FIX;");
	}	// FIX
	**********************	*/
	
	// log Uset
	log_file.print(convert_c(pController->get_set_voltage(),3).getStringUnsigned(buffer,5,2,";"));
	// log Iset
	log_file.print(convert_c(pController->get_set_current(),3).getStringUnsigned(buffer,5,2,";"));
	
	// log Uout
	log_file.print(convert_c(pController->get_output_voltage_mV(),3).getStringUnsigned(buffer,6,3,";"));
	// log Iout
	log_file.print(convert_c(pController->get_output_current_mA(),3).getStringUnsigned(buffer,6,3,";"));

	// log temperatur
	if (pController->has_temperature())
		log_file.print(convert_c(pController->get_temperature_dC(),1).getStringSigned(buffer,5,1,";"));
	/* debug */

	log_file.println();
	log_file.flush();
}	// log_data

void program_c::log_time(void)
{	// log_time
	log_file.print(convert_c(psw.current_time,3).getStringTime(buffer,true,3));
	log_file.print(";");
}	// log_time

void program_c::log_line(void)
{	// log_time
	log_file.print(convert_c(ProgramData[psw.program_counter].base.line,0).getStringUnsigned(buffer,4,0));
	log_file.print(";");
}	// log_time


/* **********************
 * **********************
 * ** helper funtions  **
 * **********************
 * ********************** */

void program_c::cpy_file_name(char * dst, const char * src)
{	// 	cpy_file_name
	uint8_t n=0;
	
	while ((dst[n]=src[n]) && (src[n]!=dot) && (n<DOS_FILE_NAME_LENGTH)) n++;
	dst[n]=NUL;
}	//	cpy_file_name

void program_c::add_file_ext(char * dst, const char *name, const char * ext)
{	// 	add extension
	while ((*dst=*name)) { dst++; name++; }
	*dst++=dot;
	while ((*dst++=*ext++));
}	//	add extension

char* program_c::skip_space(char* pChar)
{	//	skip_space
	while (	is_whitespace(*pChar)) pChar++;
	return pChar;
}	// skip_space

char* program_c::skip_non_space(char* pChar)
{	//	skip_non_space
	while (	!is_whitespace(*pChar) && (*pChar) ) pChar++;
	return pChar;
}	// skip_non_space


char* program_c::skip_n(char* pChar, uint8_t n)
{	// skkip_n
	while (n-->0) pChar++;
	return pChar;
}	// skkip_n

char program_c::upper(char Char)
{	// upper
	if ((Char>='a') && (Char<='z'))
		Char='A'+Char-'a';
	return Char;
}	// upper

uint8_t program_c::get_length(const char * Text)
{	//	get_length
	uint8_t length=0;
	
	while (*Text++) length++;

	return length;
}	//	get_length


bool program_c::is_whitespace(char Char)
{	// is_whitespace
	bool ws;
	switch (Char)
	{	// switch char
		case HT:
			/* fall through */
		case LF:
			/* fall through */
		case VT:
			/* fall through */
		case FF:
			/* fall through */
		case CR:
			/* fall through */
		case SPACE:
			ws=true;
			break;
		default:
			ws=false;
			break;
	}	// switch char
	return ws;
}	// is_whitespace

bool program_c::is_digit(char Char)
{	// is digit
	return (Char>='0') && (Char<='9');
}	// is digit

bool program_c::is_alpha(char Char)
{	// is_alpha
	char temp=upper(Char);
	return (temp>='A') && (temp<='Z');
}	// is_alpha

bool program_c::is_sign(char Char)
{	// is_sign
	return (Char=='+') || (Char=='-');
}	// is_sign


uint8_t program_c::find_token(char * Text, const char * TokenList)
{	// find_token
	uint8_t token=0;
	char 	*data;
	
	do
	{	// search token
		data=Text;
		while ( ( (upper(*data)==*TokenList) && (*TokenList) ) ||  		// match or
				( (*TokenList==SPACE) && is_whitespace(*data) ) )	// whitespace
		{	// compare with token
			data++;
			TokenList++;
		}	// compare with token
		if (*TokenList) 
		{	// no match
			while (*TokenList++);	// next entry
			data=Text;				// restart
			token++;
		}	// no match
	}	// search token
	while (*TokenList);
	
	return token;
}	// find_token


const char * program_c::get_list_text(const char* List, uint8_t entry)
{	// get_list_text
	
	while ((entry>0) && (*List))
	{	// search entry #
		entry--;			// dekrement
		while (*List++);	// next entry
	}	// search entry #
	
	return List;
}	// get_list_text


uint8_t program_c::add_list_text(char* List, const char* String)
{	// add_list_text
	uint8_t cnt=0;
	
	while (*List)
	{	// search end and count
		while (*List++);
		cnt++;
	}	// search end and count
	
	// copy data
	while ((*List++=*String++)) 
		psw.message_pointer++;

	
	// add final NUL
	*List=NUL;
	psw.message_pointer++;
	
	// return text id
	return cnt;
}	// add_list_text


uint8_t program_c::find_char(char Char, const char* String)
{	// is in
	uint8_t cnt=0;
	
	//Char=upper(Char);
	
	while ((*String) && (*String!=Char))
	{	// search char
		cnt++;
		String++;
	}	// search char
	
	if (!*String) cnt=0xFF;	// return 0xFF if not fount;
	
	return cnt;
}	// is in
