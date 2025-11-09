#ifndef __program_h__
#define __program_h__

#include <stdint.h>
#include "controller.h"
#include "parameter.h"
#include "../memory/dir.h"


class program_c:public dir_c
{
	public:
		typedef enum:uint8_t { 	CLEAR_MSG, 	// no characters, clear message line
								DISP_MSG,	// grey (dark white)
								INFO_MSG, 	// white characters
								PASS_MSG, 	// green characters
								FAIL_MSG	// red characters
							}	message_type_et;

		program_c(controller_c * Controller, parameter * Parameter );
	
		uint16_t 	getFileCount(void);
		char * 		getFileName(uint16_t FileNr=0);

		uint16_t 	getProgramFileNumber(void);	
		void		setProgramFileNumber(uint16_t FileNr);
		void		setProgramFileName(const char * pFileName);
		
		bool		process(void);
		bool		hasSD(void) { return dir_c::hasSD(); }
		bool		isBusy(void) { return 	(process_state!=IDLE) && 
											(process_state!=NO_CARD) && 
											(process_state!=RUN); }  
		
		bool		is_loaded(void) { return psw.loaded; }
		bool		has_errors(void) { return psw.error; }
		bool		is_running(void) { return process_state==RUN;}
		
		void		startProgram(bool bStart, bool bAutoStart=false);
	
		void		getMessageState(message_type_et & msg_type, uint8_t & msg_id);
		
		const char *getMessage(uint8_t msg_id);
		
	private:
		typedef enum:uint8_t {	INIT, 
								IDLE, 
								COUNT_FILES, 
								FIND_FILE, 
								OPEN_FILE, 
								LOAD_FILE, 
								PARSE_LABEL, 
								PARSE_LOOP_1, 
								PARSE_LOOP_2, 
								PARSE_MESSAGE, 
								CLOSE_FILE,
								RUN, 
								NO_CARD } state_et;
	
		typedef enum:uint8_t {	SET, 		// set command, up to 4 parameters U,I,O and delay
								WAIT, 		// wait command, one parameter (delay)
								LOOP,		// loop command either one (head) oder none (foot) parameter
								JUMP, 		// (un)conditional realtive Jump one or two parameters
								COMMENT,	// comment or empty line
								LABEL,		// target for jump
								LOG,		// log file entry
								DISP,		// same as info
								INFO,		// normal message
								PASS,		// green message
								FAIL,		// red message
								CLEAR,		// clear message
								LOAD,		// load flag data from NV memory
								SAVE,		// save flag data from NV memory
								NN
							} command_et;
		
		typedef enum:uint8_t {	OK,						// no error
								LINE_TO_LONG,			// line doaes not fit into buffer
								UNKNOWN_COMMAND,		// none of set, wait, loop, jump ':', '#'
								INVALID_LABEL_CHAR,		// non label char in sequence
								LABEL_TO_LONG,			// more than 5 label chars 
								DELAY_TO_LONG,			// >60000ms
								INVALID_DELAY,			// 0ms not a valid delay
								EXPRESSION_PENDING, 	// another assignemt is pending
								VARIABLE_MULTIPLE_USE,	// variable has already been used
								MULTIPLE_OPERATORS,		// multiple operators for one variable
								NO_VARIABLE,			// no variable selected
								IMMEDEATE_ONLY,			// immeadeate assignement only
								INVALID_PARAMETER,		// paramter not allowed
								MULTIPLE_CONDITIONS,	// multiple conditions not allowed
								INVALID_OPERATOR,		// invalid operator
								NO_OPERATOR,			// no operator selected
								OUT_OF_MEMORY,			// out of memory (message buffer) 
								EXTRA_DATA,				// unexpected data
								NO_INDEX,				// flag without index
								NO_OPEN_LOOP,			// closing loop without opening
								DUPLICATE_LABEL,		// label has been previously defined
								UNDEFINED_LABEL,		// reference label undefined
								NO_CLOSE_LOOP,			// closing loop without opening
								NO_DATA					// missing data
							} error_et;
		
		typedef enum:uint8_t { 	JUMP_UNCONDITIONAL,
								JUMP_ON_VOLTAGE_COMPARE,
								JUMP_ON_CURRENT_COMPARE,
								JUMP_ON_OUTPUT_COMPARE,
								JUMP_ON_AUTO_START_COMPARE,
								JUMP_ON_FLAG_COMPARE
							}	jump_src_et;
							
								
		typedef struct cmd_base_s	{	uint16_t	line;
										command_et	cmd;
									}	__attribute__((packed)) cmd_base_st;
		typedef struct set_cmd_s	{	uint32_t	delay;
										int16_t	 	voltage_val;
										int16_t		current_val;
										int8_t		flag_val;
										uint8_t		flag_number;
										uint8_t		voltageChange:1;
										uint8_t		voltageIncrement:1;
										uint8_t		currentChange:1;
										uint8_t		currentIncrement:1;
										uint8_t		outputChange:1;
										uint8_t		outputState:1;
										uint8_t		flagChange:1;
										uint8_t		flagIncrement;
									}	__attribute__((packed)) set_cmd_st;

		typedef struct wait_cmd_s	{	uint32_t	delay;
									}	__attribute__((packed)) wait_cmd_st;

		typedef struct loop_cmd_s	{	uint16_t	TargetPDE;
										uint16_t	Counter;
										uint8_t		level;
									}	__attribute__((packed)) loop_cmd_st;
									
		typedef struct jump_cmd_s {		uint32_t	label_code;
										uint16_t	TargetPDE;
										int16_t		Reference;
										uint8_t		flag_number;
										uint8_t		source:3;
										uint8_t		greater_than:1;
										uint8_t		less_than:1;
									}	__attribute__((packed)) jump_cmd_st;
		
		typedef struct label_cmd_s {	uint32_t	label_code;
									}	__attribute__((packed)) label_cmd_st;
									
		typedef struct log_cmd_s	{	uint8_t msg_id;	
									}	log_cmd_st;
		
		typedef struct disp_cmd_s	{	uint8_t msg_id;
										message_type_et msg_type;
									}	disp_cmd_st;
									
		typedef union cmd_data_u	{	set_cmd_st		set; 
										wait_cmd_st		wait;
										loop_cmd_st		loop;
										jump_cmd_st		jump;
										label_cmd_st	label;
										log_cmd_st		log;
										disp_cmd_st		disp;
									}	__attribute__((packed)) cmd_data_ut;
		
		typedef struct cmd_s		{	cmd_base_st		base;
										cmd_data_ut		data;
									}	__attribute__((packed)) cmd_st;

		typedef struct psw_s		{	uint16_t		program_counter;
										uint16_t		program_data_element;
										uint16_t		program_data_length;
										uint16_t		program_lines;
										uint16_t 		message_pointer;
										uint16_t		delay;
										uint32_t		last_call;
										uint32_t		current_time;
										uint8_t			loop_level;
										uint8_t			msg_id;
										message_type_et	msg_type;
										bool			error;
										bool			loaded;
										bool			auto_start;
									} 	__attribute__((packed)) psw_st;

		static const char PROGRAM_FILE_EXTENSION[];
		static const char LOG_FILE_EXTENSION[];
		static const char ERROR_FILE_EXTENSION[];
		static const char PARSE_FILE_EXTENSION[];
		static const char ERROR_TEXT[];
		static const char CMD_TEXT[];
		static const char LABEL_CHARS[];
		static const char JUMP_CHARS[];
		static const char SET_CHARS[];
		static const uint16_t MaxProgramLength=512;
		static const uint16_t MaxMessageBufferLength=1024;
		static const uint8_t BufferLen=20;
		static const uint8_t PrgLineLen=80;
		static const uint16_t MaxDelay=60000;
		static const uint8_t MaxFlagCnt=10;

		static const uint8_t LabelLength=5;
		

		cmd_st 	ProgramData[MaxProgramLength];
		char 	Messages[MaxMessageBufferLength];
		uint8_t	Flags[MaxFlagCnt];
		psw_st psw;
		
		controller_c * pController;
		parameter	 * pParameter;
		
	
		state_et	process_state;
		char		file_name[DOS_FILE_NAME_LENGTH+1];
		char 		temp_file_name[DOS_FILE_NAME_LENGTH+1];
		char		program_file_name_ext[DOS_FILE_NAME_EXT_LENGTH+1];
		char 		log_file_name_ext[DOS_FILE_NAME_EXT_LENGTH+1];
		char 		error_file_name_ext[DOS_FILE_NAME_EXT_LENGTH+1];
		char 		parse_file_name_ext[DOS_FILE_NAME_EXT_LENGTH+1];
		char   		buffer[BufferLen+1];
		char		prg_line[PrgLineLen+1];
		uint16_t	file_cnt,file_number,temp_file_number;
		File		program_file;
		File		log_file;
		File		error_file;
		File		parse_file;
		bool		verbose;

		void clear_program_data(void);
		void open_program_file(void);
		void close_porgram_file(void);
		bool read_program_file_line(void);
		void parse_line(void);
		void parse_set(char * pData);
		void parse_wait(char * pData);
		void parse_loop(char * pData);
		void parse_jump(char * pData);
		void parse_label(char * pData);
		void parse_log(char * pData);
		void parse_disp(char * pData);
		void parse_load(char * pData);
		void parse_save(char * pData);

		bool check_label(void);
		bool check_loop(void);
		bool match_loop(void);
		bool list_message(void);
		
		void process_program(void);
		void process_set  (void);
		void process_wait (void);
		void process_loop (void);
		void process_jump (void);
		void process_log  (void);
		void process_disp (void);
		void process_load (void);
		void process_save (void);
		void process_error(void);
		
		void log_header(void);
		void log_message(void);
		void log_data(void);
		void log_time(void);
		void log_line(void);

		
		jump_src_et calc_jump_source(char * source);
		uint32_t calc_label(char * pData);
		uint8_t calc_flag_number(char * &source);
		
		void log_error(error_et error, uint16_t line);
		
		void cpy_file_name(char * dst, const char * src);
		void add_file_ext(char * dst, const char *name, const char * ext);
		char* skip_space(char* pChar);
		char* skip_non_space(char* pChar);
		char* skip_n(char* pChar, uint8_t n);
		char upper(char Char);
		uint8_t get_length(const char * Text);

		bool is_whitespace(char Char);
		bool is_digit(char Char);
		bool is_alpha(char Char);
		bool is_sign(char Char);
		
		uint8_t find_token(char * Text, const char * TokenList);
		const char * get_list_text(const char* List, uint8_t entry);
		uint8_t add_list_text(char* List, const char* String);
		
		uint8_t find_char(char Char, const char* String);
};



#endif // __program_h__
