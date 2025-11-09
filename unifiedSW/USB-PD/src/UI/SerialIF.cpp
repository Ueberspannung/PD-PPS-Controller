#include "SerialIF.h"
#include "../ASCII/ASCII_ctrl.h"
#include "../tool/version.h"
#include "../tool/convert.h"

/*********************************
 * Command List
 * Type	Cmd	Comment
 *  R	 !	status
 * 	RW	 #	current profile
 *  R	 ?	output status: Switch, Vbus, Vout, Iout
 *  RW	 C	current calibration
 * 	W	 E	erase parameter
 *  R	 F 	find closest Fix voltage
 *  RW	 I	max. available current at specific voltage
 *  RW	 L  logging
 *  R	 N	number of availabele profiles / profile data
 *  RW	 O	output switch status
 *  RW	 R	select / read regulator mode
 *  RW	 S	select / read startup parameter
 * 	R	 T	read current power type
 * 	RW	 U  select / read voltage
 *  R	 V	read version / built date
 */

const char SerialIF_c::ReadCommands[]		="!#?CFILNORSTUV";
const char SerialIF_c::WriteCommands[]		="#?CEILORSU";

const char SerialIF_c::StatusTextReady[]	="ready";
const char SerialIF_c::StatusTextBusy[]		="busy";
const char SerialIF_c::StatusTextChange[]	="changing power";
const char SerialIF_c::StatusTextErase[]	="erasing parameter";
const char SerialIF_c::StatusTextPower[]	="waiting for PD";
const char SerialIF_c::StatusTextParameter[]="reading parameter";

const char SerialIF_c::RegulatorModes[]		="OFF\0CV\0CVCC\0CVCC^\0";
const char SerialIF_c::AutoStartModes[]		="OFF\0SET\0ON\0";
const char SerialIF_c::PowerModes[]			="N/A\0FIX\0PPS\0";

void SerialIF_c::init(	SerialClass * com, 
						controller_c * Controller, 
						parameter * Parameter, 
						log_c * Log,
						program_c * Program)
{	// init
	pPort=com;
	pController=Controller;
	pParameter=Parameter;
	pLog=Log;
	pProgram=Program;
	externalUpdate=false;
	bPauseAutoSend=false;
}	// init


void SerialIF_c::begin(void)
{	// begin
	// connection established
	processMode=START;
	BufferCnt=0;
	SendIntervall=0;
}	// begin

void SerialIF_c::end(void)
{	// end
	// disconnected
}	// end

void SerialIF_c::pauseAutoSend(bool bPause)
{	// pauseAutoSend
	bPauseAutoSend=bPause;
}	// pauseAutoSend

bool SerialIF_c::process(void)
{	// process
    bool tempUpdate;

	TimeStamp=pController->clock_ms();

	switch (processMode)
	{	// switch processMode
		case START:
			processMode=PARAMETER;
			break;
		case PARAMETER:
			if (!pParameter->process())
			{
				processMode=CONTROLLER;
			}
			break;
		case CONTROLLER:
			if (pController->is_up())
			{
				processMode=POWER;
			}
			break;
		case POWER:
			if (pController->is_power_ready())
			{
				processMode=WAITING;
			}
			break;
		case ERASE:
			/* FALL THROUH */
		case CHANGE:
			/* FALL THROUH */
		case WAITING:
			/* FALL THROUH */
			if (bReceive())
				DecodeCommand();
			else if ((BufferCnt==0) && (SendIntervall>0) && !bPauseAutoSend)
			{	// check auto output status
				if (((uint16_t)(TimeStamp-LastSend))>SendIntervall)
				{	// send data
					Buffer[0]=ReadFlag;
					doOutputStatus();
					addChar(Buffer,BufferCnt,CR);
					pPort->write(Buffer,BufferCnt);
					BufferCnt=0;
				}	// send data
			}	// check auto output status
			break;
		default:
			processMode=START;
			break;
	}	// switch processMode


    /*************************************************************
     * if externalUpdate flag is set and current state is waiting
     * report update rrequest und clear externalUpdate flag
     */

	tempUpdate=externalUpdate && (processMode==WAITING);
	externalUpdate=externalUpdate && !tempUpdate;
	return tempUpdate;
}	// process

bool SerialIF_c::bReceive(void)
{	// receive
	bool bOk=false;
	while (pPort->available() && !bOk)
	{
		Buffer[BufferCnt]=pPort->read();
		bOk=Buffer[BufferCnt]==CR;
		if (!bOk) BufferCnt=(BufferCnt+1)%BufferLen;
	}
	return bOk;
}	// receive

void SerialIF_c::DecodeCommand(void)
{	// decode command
	bool bOk;

	// check if min cmd legnth is ok
	bOk=BufferCnt>=RxMin;
	// check if read/write selector is ok
	if (bOk)
		bOk=(Buffer[0]==ReadFlag) || (Buffer[0]==WriteFlag);
	// check if valid command has been supplied
	if (bOk)
	{
		if (Buffer[0]==ReadFlag)
			bOk=bFind(Buffer[1],ReadCommands);
		else
			bOk=bFind(Buffer[1],WriteCommands);
	}

	if (bOk)
	{	// try to decode command
		if (processMode==WAITING)
		{	// idle, decode
			switch (Buffer[1])
			{	// switch cmd
				case '!':	//  R	 !	status
					doSystemStatus();
					break;
				case '#':	// 	RW	 #	read /set profile
					doProfile();
					break;
				case '?':	//  R	 ?	output status: Switch, Vbus, Vout, Iout
					doOutputStatus();
					break;
				case 'C':	//  RW	 C	current calibration
					doCurrentCalibration();
					break;
				case 'E':	// 	W	 E	erase parameter
					doEraseParameter();
					break;
				case 'F':	//  R	 F 	find closest Fix voltage
					doFindProfile();
					break;
				case 'I':	//  RW	 I	max. available current at specific voltage
					doCurrent();
					break;
				case 'L':
					doLogging();
					break;
				case 'N':	//  R	 N	number of availabele profiles / profile data
					doPowerInfo();
					break;
				case 'O':	//  RW	 O	output switch status
					doOutputSwitch();
					break;
				case 'R':	//  RW	 R	select / read regulator mode
					doRegulatorSettings();
					break;
				case 'S':	//  RW	 S	select / read startup parameter
					doStartUpSettings();
					break;
				case 'T':	// 	R	 T	read current power type
					doPowerType();
					break;
				case 'U':	// 	RW	 U  select / read  voltage
					doVoltage();
					break;
				case 'V':	//  R	 V	read version / built date
					doVersion();
					break;
				default:
					// gnore, no reply
					BufferCnt=0;
					break;
			}	// switch cmd

		}	// idle, decode
		else
		{	// busy
			doSystemStatus();
		}	// busy

		if (BufferCnt)
		{
			addChar(Buffer,BufferCnt,CR);
			pPort->write(Buffer,BufferCnt);
		}

	}	// try to decode command

	BufferCnt=0;
}	// decode command

bool SerialIF_c::bFind(const char Val, const char * Set)
{	// find
	bool bFound=false;

	while (!bFound && *Set)
	{
		bFound=*Set==Val;
		Set++;
	}
	return bFound;
}	// find

void SerialIF_c::cpy(char * dest, uint8_t & destPos, const char *src)
{	// copy string
	while (*src) dest[destPos++]=*src++;
}	// copy string

void SerialIF_c::addChar(char * dest, uint8_t & destPos, const char Val, uint8_t cnt)
{	// add specific character
	while (cnt--) dest[destPos++]=Val;
}	// add specific character


uint8_t SerialIF_c::CountData(const char * Data)
{	// CountData
	uint8_t n=0;
	while (*Data)
	{	// Valid String
		n++;
		while (*Data++); 	// skip until next
	}	// Valid String
	return n;
}	// CountData

const char * SerialIF_c::NextItem(const char * Data)
{	// NextItem
	while (*Data++); 	// skip until next
	return Data;
}	// NextItem

const char * SerialIF_c::GetItem(const char * Data,uint8_t n)
{	// GetMenuItem
	while (n--) Data=NextItem(Data); 	// skip until next
	return Data;
}	// GettItem

uint8_t SerialIF_c::FindItem(const char * Data, const char * Items)
{	// FindItem
	uint8_t n=0;
	const char * search;
	while (*Items)
	{	// data to process
		search=Data;
		while ((*Items==*search) && (*Items) && (*search))
		{	// compare
			search++;
			Items++;
		}	// compare

		if (!*Items)
		{	// compared till end, check
			if ((*search!=CR) && (*search!=SPACE))
			{	// not found
				n++;
				Items++;		// skip NUL
			}	// not found
		}	// compared till end, check
		else
		{	// no match
			n++;
			while(*Items++);	// skip rest
		}	// no match
	}	// data to process
	return n;
}	// FindItem

void SerialIF_c::doSystemStatus(void)
{	// doSystemStatus
	BufferCnt=2;
	
	addChar(Buffer,BufferCnt,SPACE);

	switch (processMode)
	{
		case START:
			break;
		case PARAMETER:
			break;
		case CONTROLLER:
			/* fall through */
		case POWER:
			break;
		case ERASE:
			if (!pParameter->process())
				processMode=WAITING;
			break;
		case CHANGE:
			if (pController->is_power_ready())
				processMode=WAITING;
			break;
		default:
			break;
	}

	if (processMode==WAITING)
	{
		cpy(Buffer,BufferCnt,StatusTextReady);
	}
	else
	{
		cpy(Buffer,BufferCnt,StatusTextBusy);
		if (processMode!=START)
			addChar(Buffer,BufferCnt,SPACE);
	}
	switch (processMode)
	{
		case PARAMETER:
			cpy(Buffer,BufferCnt,StatusTextParameter);
			addChar(Buffer,BufferCnt,SPACE);
			convert_c(pParameter->getProgress(),0).getStringUnsigned(&Buffer[BufferCnt],3,0);
			BufferCnt+=3;
			addChar(Buffer,BufferCnt,'%');
			break;
		case CONTROLLER:
			/* fall through */
		case POWER:
			cpy(Buffer,BufferCnt,StatusTextPower);
			break;
		case ERASE:
			cpy(Buffer,BufferCnt,StatusTextErase);
			addChar(Buffer,BufferCnt,SPACE);
			convert_c(pParameter->getProgress(),0).getStringUnsigned(&Buffer[BufferCnt],3,0);
			BufferCnt+=3;
			addChar(Buffer,BufferCnt,'%');
			break;
		case CHANGE:
			cpy(Buffer,BufferCnt,StatusTextChange);
			break;
		default:
			break;
	}

}	// doSystemStatus

void SerialIF_c::doProfile(void)
{	// doProfile
	if (Buffer[0]==WriteFlag)
	{	// set profile
		uint8_t profile;
		profile=convert_c(&Buffer[2]).getUnsigned(0);
		if (profile!=pController->get_current_profile())
		{	// new profile

			PD_power_info_t PowerInfo;
			uint16_t outputVoltage,outputCurrent;
			outputCurrent=pController->get_set_current();
			outputVoltage=pController->get_set_voltage();
			pController->get_profile_info(profile,&PowerInfo);


			if (PowerInfo.type!=PD_PDO_TYPE_FIXED_SUPPLY)
			{	// pps, check limits
				if (outputVoltage>PowerInfo.max_v)
					outputVoltage=PowerInfo.max_v;
				if (outputVoltage<PowerInfo.min_v)
					outputVoltage=PowerInfo.min_v;
				if (outputCurrent>pController->get_max_current_mA(outputVoltage))
					outputCurrent=pController->get_max_current_mA(outputVoltage);
				if (outputCurrent<pController->get_min_current_mA())
					outputCurrent=pController->get_min_current_mA();
			}	// pps, check limits
			else
			{	// fixed profile, set
					outputVoltage=PowerInfo.max_v;
					outputCurrent=PowerInfo.max_i;
			}	// fixed profile, set

			if (pParameter->getAutoSet())
			{	// save setting for next start
					pParameter->setVoltage_mV(outputVoltage);
					pParameter->setCurrent_mA(outputCurrent);
					pParameter->setModePPS(	PowerInfo.type !=
											PD_PDO_TYPE_FIXED_SUPPLY);
					pParameter->write();
			}	// save setting for next start


			if (PowerInfo.type == PD_PDO_TYPE_FIXED_SUPPLY)
				pController->set_voltage(outputVoltage);
			else
				pController->set_power(outputVoltage,outputCurrent);

			processMode=CHANGE;
			externalUpdate=true;
			doSystemStatus();
		}	// new profile
	}	// set profile
	else
	{	// just get current profile
		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE);

		Buffer[BufferCnt++]='0'+pController->get_current_profile();
	}	// just get current profile
}	// doProfile

void SerialIF_c::doOutputStatus(void)
{	// doOutputStatus
	if (Buffer[0]==ReadFlag)
	{	// send output status
		LastSend+=SendIntervall;
		BufferCnt=0;
		addChar(Buffer,BufferCnt,'?');
		addChar(Buffer,BufferCnt,'?');
		addChar(Buffer,BufferCnt,SPACE);
		
		if (!pController->is_output_enabled())
			addChar(Buffer,BufferCnt,'0');
		else if (pController->is_constant_current_active())
			addChar(Buffer,BufferCnt,'!');
		else
			addChar(Buffer,BufferCnt,'1');
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_Vbus_mV(),
					millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
														millisLen,millisDecimals);
		BufferCnt+=millisLen;
		addChar(Buffer,BufferCnt,'V');
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_output_voltage_mV(),
					millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
														millisLen,millisDecimals);
		BufferCnt+=millisLen;
		addChar(Buffer,BufferCnt,'V');
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_output_current_mA(),
					millisDecimals).getStringSigned(	&Buffer[BufferCnt],
														millisLen,millisDecimals);
		BufferCnt+=millisLen;
		addChar(Buffer,BufferCnt,'A');
		if (pController->has_temperature())
		{ // add temperature
			addChar(Buffer,BufferCnt,SPACE);
			convert_c(	pController->get_temperature_dC(),
						decisDecimals).getStringSigned(	&Buffer[BufferCnt],
														decisLen,decisDecimals);
			BufferCnt+=decisLen;
		} // add temperature
		addChar(Buffer,BufferCnt,'C');
	}	// send output status
	else
	{	// set output status timer
		SendIntervall=convert_c(&Buffer[2]).getUnsigned(0);
		if (SendIntervall<minSendIntervall) SendIntervall=0;
		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	SendIntervall,0).getStringUnsigned(	&Buffer[BufferCnt],5,0);
		BufferCnt+=5;
		addChar(Buffer,BufferCnt,'m');
		addChar(Buffer,BufferCnt,'s');
		LastSend=pController->clock_ms();
	}	// set output status timer
}	// doOutputStatus

void SerialIF_c::doCurrentCalibration(void)
{	// doCurrentCalibration
	uint16_t internal,reference;
	const char * data=&Buffer[2];
	if (Buffer[0]==WriteFlag)
	{	// set current calibration
		internal=0;
		reference=0;
		internal=convert_c(data).getUnsigned(millisDecimals);
		// find next val
		while (*data==SPACE) data++;
		while (*data!=SPACE) data++;
		reference=convert_c(data).getUnsigned(millisDecimals);
		pController->set_output_current_calibration(internal,reference);
		pParameter->setCalI(internal,reference);
		pParameter->write();
	}	// set current calibration

	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	
	pParameter->getCalI(&internal,&reference);
	convert_c(	internal,
				millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
													millisLen,millisDecimals);
	BufferCnt+=millisLen;
	addChar(Buffer,BufferCnt,'A');
	addChar(Buffer,BufferCnt,SPACE);
	convert_c(	reference,
				millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
													millisLen,millisDecimals);
	BufferCnt+=millisLen;
	addChar(Buffer,BufferCnt,'A');
}	// doCurrentCalibration

void SerialIF_c::doEraseParameter(void)
{	// doEraseParameter
	pParameter->clear(true);
	processMode=ERASE;
	externalUpdate=true;
	doSystemStatus();
}	// doEraseParameter

void SerialIF_c::doFindProfile(void)
{	// doFindProfile
	uint16_t voltage;
	voltage=convert_c(&Buffer[2]).getUnsigned(millisDecimals);
	voltage=pController->get_fix_voltage_mV(voltage);
	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	convert_c(	voltage,
				millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
													ProfileVoltage,
													ProfileDecimals,
													"V");
	BufferCnt+=ProfileVoltage+1;
}	// doFindProfile

void SerialIF_c::doCurrent(void)
{	// doFindCurrent
	bool sendSetCurrent=false;

	if (BufferCnt==2)
	{	// no parameter, report current
		sendSetCurrent=true;
	}	// no parameter, report current
	else if (Buffer[0]==WriteFlag)
	{	// set current
		if (pController->is_PPS())
		{	// current setting only possible in PPS mode
			uint16_t voltage,current;
			current=convert_c(&Buffer[2]).getUnsigned(millisDecimals);
			current-=current%10;
			voltage=pController->get_set_voltage();
			if (current>pController->get_max_current_mA(voltage))
				current=pController->get_max_current_mA(voltage);

			if (current!=pController->get_set_current())
			{ // new Value, set
				pController->set_power(voltage,current);
				if (pParameter->getAutoSet())
				{	// Auto -> Save
					pParameter->setCurrent_mA(current);
					pParameter->write();
				}	// Auto -> Save
				BufferCnt=2;
				addChar(Buffer,BufferCnt,SPACE);
				convert_c(	current,
							millisDecimals).getStringUnsigned( 	&Buffer[BufferCnt],
																ProfileCurrent,
																ProfileDecimals,
																"A");
				BufferCnt+=ProfileCurrent+1;
                externalUpdate=true;
			} // new Value, set
			else
			{
				sendSetCurrent=true;
			}
		}	// current setting only possible in PPS mode
		else
		{
			sendSetCurrent=true;
		}
	}	// set voltage
	else
	{	// search for max current
		uint16_t voltage;
		voltage=convert_c(&Buffer[2]).getUnsigned(millisDecimals);
		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_max_current_mA(voltage),
					millisDecimals).getStringUnsigned( 	&Buffer[BufferCnt],
														ProfileCurrent,
														ProfileDecimals,
														"A");
		BufferCnt+=ProfileCurrent+1;

	}	// search for max voltage

	if (sendSetCurrent)
	{	// send set voltage
		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_set_current(),
					millisDecimals).getStringUnsigned( 	&Buffer[BufferCnt],
														ProfileCurrent,
														ProfileDecimals,
														"A");
		BufferCnt+=ProfileCurrent+1;
	}	// send set current
}	// doCurrent

void SerialIF_c::doPowerInfo(void)
{	// doProfile
	if (BufferCnt==2)
	{	// no parameter
		// report number of profiles
		addChar(Buffer,BufferCnt,SPACE);
		Buffer[BufferCnt++]='0'+pController->get_profile_cnt();
	}	// no parameter
	else
	{	// report profile info
		uint8_t n;
		PD_power_info_t PowerInfo;

		n=convert_c(&Buffer[2]).getUnsigned(0);

		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE,BufferLen);
		BufferCnt=3;
		Buffer[BufferCnt++]='#';
		Buffer[BufferCnt++]='0'+n;
		Buffer[BufferCnt++]=':';
		Buffer[BufferCnt++]=' ';


		if (pController->get_profile_info(n++,&PowerInfo))
		{	// Profiles to process
			memcpy(	&Buffer[BufferCnt],
					pController->get_profile_text(PowerInfo.type),3);

			BufferCnt+=3;
			addChar(Buffer,BufferCnt,SPACE);

			switch (PowerInfo.type)
			{	// Type
				case PD_PDO_TYPE_FIXED_SUPPLY:
					convert_c(	PowerInfo.max_v,
								millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
																	ProfileVoltage,
																	ProfileDecimals,
																	"V");
					BufferCnt+=ProfileVoltage+1;
					addChar(Buffer,BufferCnt,SPACE);
					addChar(Buffer,BufferCnt,'/');
					addChar(Buffer,BufferCnt,SPACE);
					convert_c(	PowerInfo.max_i,
								millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
																	ProfileCurrent,
																	ProfileDecimals,
																	"A");
					BufferCnt+=ProfileCurrent+1;
					break;
				case PD_PDO_TYPE_BATTERY:
					break;
				case PD_PDO_TYPE_VARIABLE_SUPPLY:
					break;
				case PD_PDO_TYPE_AUGMENTED_PDO:
					convert_c(	PowerInfo.min_v,
								millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
																	ProfileVoltage,
																	ProfileDecimals);
					BufferCnt+=ProfileVoltage;
					addChar(Buffer,BufferCnt,SPACE);
					addChar(Buffer,BufferCnt,'-');
					addChar(Buffer,BufferCnt,SPACE);
					convert_c(	PowerInfo.max_v,
								millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
																	ProfileVoltage,
																	ProfileDecimals,
																	"V");
					BufferCnt+=ProfileVoltage+1;
					addChar(Buffer,BufferCnt,SPACE);
					addChar(Buffer,BufferCnt,'/');
					addChar(Buffer,BufferCnt,SPACE);
					convert_c(	PowerInfo.max_i,
								millisDecimals).getStringUnsigned(	&Buffer[BufferCnt],
																	ProfileCurrent,
																	ProfileDecimals,
																	"A");
					BufferCnt+=ProfileCurrent+1;
					break;
				default:
					break;
			}	// Type
		}	// Profiles to process
		else
		{	// not available
			Buffer[BufferCnt++]='N';
			Buffer[BufferCnt++]='/';
			Buffer[BufferCnt++]='A';
		}

	}	// report profile info
}	// doProfile

void SerialIF_c::doOutputSwitch(void)
{	// doOutputSwitch
	if (Buffer[0]==WriteFlag)
	{	// set Output Switch
		uint8_t val=convert_c(&Buffer[2]).getUnsigned(0);
		pController->enable_output(val!=0);
		if (pParameter->getAutoOn())
		{
			pParameter->setAutoOn(val!=0);
			pParameter->write();
		}
		externalUpdate=true;
	}	//	set Output Switch

	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	Buffer[BufferCnt++]=(pController->is_output_enabled())?('1'):('0');
}	// doOutputSwitch

void SerialIF_c::doRegulatorSettings(void)
{	// doRegulatorSettings
	if (Buffer[0]==WriteFlag)
	{	//	set Mode
		const char * pArg;
		controller_c::operating_mode_et mode;
		pArg=&Buffer[2];
		while (*pArg==SPACE) pArg++;
		mode=(controller_c::operating_mode_et)FindItem(pArg,RegulatorModes);
		if (mode!=controller_c::CONTROLLER_MODE_end)
		{	// valid mode
			pController->set_operating_mode(mode);
			if (pParameter->getAutoSet())
			{	// sichern
				pParameter->setCVCC(mode);
				pParameter->write();
			}	// sichern
		}	// valid mode
		externalUpdate=true;
	}	// set mode

	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	cpy(Buffer,BufferCnt,GetItem(RegulatorModes,(uint8_t)pController->get_operating_mode()));
}	// doRegulatorSettings

void SerialIF_c::doStartUpSettings(void)
{	// doStartUpSettings
	uint8_t mode;
	if (Buffer[0]==WriteFlag)
	{	//	set Mode
		const char * pArg;
		pArg=&Buffer[2];
		bool bAutoSet,bAutoOn;
		while (*pArg==SPACE) pArg++;
		mode=FindItem(pArg,AutoStartModes);
		bAutoSet=(mode==1) || (mode==2);
		bAutoOn=mode==2;
		if ((pParameter->getAutoOn()!=bAutoOn) ||
			(pParameter->getAutoSet()!=bAutoSet))
		{	// new values, set
			pParameter->setAutoOn(bAutoOn);
			pParameter->setAutoSet(bAutoSet);
			pParameter->write();
		}	// new values, set
	}	// set mode

	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	if (pParameter->getAutoOn())
		mode=2;
	else if (pParameter->getAutoSet())
		mode=1;
	else mode=0;
	cpy(Buffer,BufferCnt,GetItem(AutoStartModes,mode));
}	// doStartUpSettings

void SerialIF_c::doPowerType(void)
{	// doPowerType
	const char * pMode=PowerModes;
	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);

	if (pController->is_PD())
		pMode=NextItem(pMode);
	if (pController->is_PPS())
		pMode=NextItem(pMode);

	cpy(Buffer,BufferCnt,pMode);
}	// doPowerType

void SerialIF_c::doVoltage(void)
{	// doFixVoltage
	bool sendSetVoltage=false;
	if (BufferCnt==2)
	{	// no parameter, report voltage
		sendSetVoltage=true;
	}	// no parameter, report voltage
	else if (Buffer[0]==WriteFlag)
	{	// set voltage
		if (pController->is_PPS())
		{	// Voltage setting only possible in PPS mode
			uint16_t voltage,current;
			voltage=convert_c(&Buffer[2]).getUnsigned(millisDecimals);
			voltage-=voltage%20;
			current=pController->get_set_current();

			if (voltage<pController->get_min_voltage_mV())
				voltage=pController->get_min_voltage_mV();
			if (voltage>pController->get_max_voltage_mV(current))
				voltage=pController->get_max_voltage_mV(current);

			if (voltage!=pController->get_set_voltage())
			{ // new Value, set
				pController->set_power(voltage,current);
				if (pParameter->getAutoSet())
				{	// Auto -> Save
					pParameter->setVoltage_mV(voltage);
					pParameter->write();
				}	// Auto -> Save

				BufferCnt=2;
				addChar(Buffer,BufferCnt,SPACE);
				convert_c(	voltage,
							millisDecimals).getStringUnsigned( 	&Buffer[BufferCnt],
																ProfileVoltage,
																ProfileDecimals,
																"V");
				BufferCnt+=ProfileVoltage+1;
                externalUpdate=true;
			} // new Value, set
			else
				sendSetVoltage=true;
		}	// Voltage setting only possible in PPS mode
		else
			sendSetVoltage=true;
	}	// set voltage
	else
	{	// search for max voltage
		uint16_t current;
		current=convert_c(&Buffer[2]).getUnsigned(millisDecimals);
		current-=current%10;

		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_max_voltage_mV(current),
					millisDecimals).getStringUnsigned( 	&Buffer[BufferCnt],
														ProfileVoltage,
														ProfileDecimals,
														"V");
		BufferCnt+=ProfileVoltage+1;

	}	// search for max voltage

	if (sendSetVoltage)
	{	// send set voltage
		BufferCnt=2;
		addChar(Buffer,BufferCnt,SPACE);
		convert_c(	pController->get_set_voltage(),
					millisDecimals).getStringUnsigned( 	&Buffer[BufferCnt],
														ProfileVoltage,
														ProfileDecimals,
														"V");
		BufferCnt+=ProfileVoltage+1;
	}	// send set voltage
}	// doFixVoltage

void SerialIF_c::doVersion(void)
{	// get version
	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	cpy(Buffer,BufferCnt,Version_c().pgmGetVersion());
	addChar(Buffer,BufferCnt,SPACE);
	cpy(Buffer,BufferCnt,Version_c().pgmGetBuiltDate());
}	// get version

void SerialIF_c::doLogging(void)
{	// get / set Logging
	uint32_t logFileNum=0;

	if (Buffer[0]==WriteFlag)
	{	// configure Logging
		uint16_t logInterval_ms;
		log_c::logInterval_et logInterval=log_c::LOG_30000ms; 

		logInterval_ms=convert_c(&Buffer[2]).getUnsigned(logDecimals);
		while (logInterval_ms<pLog->getLogInterval_ms(logInterval))
			logInterval=(log_c::logInterval_et)((uint8_t)logInterval-1);
		pLog->setInterval(logInterval);
		pParameter->setLogInteval(logInterval);
		pParameter->write();
		externalUpdate=true;
	}	// configure Logging
	
	BufferCnt=2;
	addChar(Buffer,BufferCnt,SPACE);
	convert_c(	pLog->getLogInterval_ms(pParameter->getLogInterval()),
				logDecimals).getStringUnsigned(	&Buffer[BufferCnt],
												logIntervalLen,
												logDecimals,
												"ms");
	BufferCnt+=logIntervalLen+2;
	addChar(Buffer,BufferCnt,SPACE);
	if (pLog->hasSD()) logFileNum=pLog->getLogFileNumber();
	convert_c(	logFileNum,
				logDecimals).getStringUnsignedZeros(&Buffer[BufferCnt],
													logFileLen,
													logDecimals);
	BufferCnt+=logFileLen;
}	// get / set Logging


