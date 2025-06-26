//#include <Arduino.h>
#include "parameter.h"
#include "src/ASCII/ASCII_ctrl.h"
/*
#define PRINTLN(x) 		Serial1.println(x)
#define PRINTLN_HEX(x) 	Serial1.println(x,HEX)
#define PRINT(x)		Serial1.print(x)
#define PRINT_HEX(x)	Serial1.print(x,HEX)
*/
#define PRINTLN(x)
#define PRINTLN_HEX(x)
#define PRINT(x)
#define PRINT_HEX(x)

const uint8_t PROGMEM parameter::pgmFF[]={	0xFF, 0xFF, 0xFF, 0xFF,
											0xFF, 0xFF, 0xFF, 0xFF,
											0xFF, 0xFF, 0xFF, 0xFF,
											0xFF, 0xFF, 0xFF, 0xFF };
const char parameter::configFileName[]= 	"PD-PPS.SET";
const char parameter::configIDList[]=		"VoltageCalibrationReading_mV=\0"	// ITEM_CAL_U ,
											"VoltageCalibrationReference_mV=\0"	// ITEM_REF_U,
											"CurrentCalibrationReading_mA=\0"	// ITEM_CAL_I,
											"CurrentCalibrationReference_mA=\0"	// ITEM_REF_I,
											"OutputVoltageSetting_mV=\0"		// ITEM_SET_U,
											"OutputCurrentSetting_mA=\0"		// ITEM_SET_I,
											"DisplayBrightness_percentage=\0"	// ITEM_BRIGHTNESS,
											"PPS-Mode_on (0/1)=\0"				// ITEM_PPS,
											"AutomaticSetup_on (0/1)=\0"		// ITEM_AUTO_SET,
											"AutomaticPower_on (0/1)=\0"		// ITEM_AUTO_ON,
											"RegulatorMode (0-3)=\0"			// ITEM_REGULATOR,
											"\0";								// ITEM_END

parameter::parameter()
{	// parameter
	setInitData();
	initPos();
	ParameterPos=0;
	Address		=0;
	memoryType	=NO_MEMORY;
	sdState     =SD_IDLE;
	parameterIterator=ITEM_END;
}	// parameter		parameter_st Parameter;


bool parameter::process(void)		// returns true if busy
{	// processe

	switch (paramState)
	{	//	switch (paramState)
		case para_start:
			PRINTLN(F("param_start"));
			process_para_start();
			break;
		case para_init:
			PRINTLN(F("para_init"));
			process_para_init();
			break;
		case para_clear:
			PRINT(F("para_clear "));
			PRINT_HEX(Address);
			PRINT(F(" / "));
			PRINT_HEX(Eeprom.getAddressSpace());
			PRINT(F(" : "));
			PRINT(getProgress());
			PRINTLN(F("%"));
			process_para_clear();
			break;
		case para_write_flag:
			PRINTLN(F("para_write_flag"));
			process_para_write_flag();
			break;
		case para_write_first:
			PRINTLN(F("para_write_first"));
			process_para_write_first();
			break;
		case para_write:
			PRINTLN(F("para_write"));
			process_para_write();
			break;
		case para_read:
			PRINT(F("para_read ( "));
			PRINT(lowPos);
			PRINT(F(" / "));
			PRINT(midPos);
			PRINT(F(" / "));
			PRINT(highPos);
			PRINTLN(F(" )"));
			process_para_read();
			break;
		case para_idle:
			break;
		default:
			break;
	}	//	switch (paramState)
	return isBusy();
}	// processe

uint8_t parameter::getProgress(void)
{	// getProgress
	uint16_t progress=100;
	switch (paramState)
	{	//	switch (paramState)
		case para_start:
			progress=0;
			break;
		case para_init:
			progress=1;
			break;
		case para_clear:
			progress=calcPercentage(Address,Eeprom.getAddressSpace())+1;
			if (progress>98) progress=98;
			break;
		case para_write_flag:
			progress=99;
			break;
		case para_write_first:
			progress=100;
			break;
		case para_write:
			progress=100;
			break;
		case para_read:
			switch (memoryType)
			{	// memory Type
				case EEPROM:
					progress=calcPercentage(calcParamCnt()-(highPos-lowPos),calcParamCnt());
					break;
				case SDCARD:
					progress=calcPercentage(configFile.position(),configFile.size());
					break;
				default:
					progress=100;
			}	// memory Type
			break;
		case para_idle:
			progress=100;
			break;
		default:
			progress=100;
			break;
	}	//	switch (paramState)

	return (uint8_t) progress;
}	//	getProgress

void parameter::read(void)
{	// read
	paramState=para_init;
}	// read

void parameter::write(void)
{	// write
	paramState=para_write;
	switch (memoryType)
	{
		case SDCARD:
			if (hasSD())
				sdState=SD_START;
			else
				memoryType=NO_MEMORY;
			break;
		default:
			break;
	}
}	// write

void parameter::clear(bool bSetInitData)
{	// clear
	if (bSetInitData)
		setInitData();
	Address=0;
	paramState=para_clear;
}	// clear

void parameter::setBrightness(uint8_t brightness)
{	// set brightness value
	Parameter.Brightness=brightness;
}	// set brightness value

void parameter::setAutoSet(bool bAuto)
{	// setAutoSet
	Parameter.autoSet=bAuto;
}	// setAutoSet

void parameter::setAutoOn(bool bAuto)
{	//	setAutoOn
	Parameter.autoOn=bAuto;
}	//	setAutoOn

void parameter::setModePPS(bool bPPS)
{	// 	setModePPS
	Parameter.PPS=bPPS;
}	//	setModePPS

void parameter::setCVCC(controller_c::operating_mode_et mode)
{	// setCVCC
	Parameter.CVCC=(uint8_t)mode;
}	// setCVCC

void parameter::setCalU(uint16_t Cal, uint16_t Ref)
{	// setCalU
	Parameter.CalU=Cal;
	Parameter.RefU=Ref;
}	// setCalU

void parameter::setCalI(uint16_t Cal, uint16_t Ref)
{	// setCalI
	Parameter.CalI=Cal;
	Parameter.RefI=Ref;
}	// setCalI


void parameter::setVoltage_mV(uint16_t VoltageStep)
{	//	setVoltageStep
	Parameter.setU=VoltageStep;
}	//	setVoltageStep

void parameter::setCurrent_mA(uint16_t CurrentStep)
{	//	setVoltageStep
	Parameter.setI=CurrentStep;
}	//	setVoltageStep

uint8_t parameter::getBrightness(void)
{	// get Brightness
	return Parameter.Brightness;
}	// get Brightness

bool parameter::getAutoSet(void)
{	// 	getAutoSet
	return Parameter.autoSet;
}	// 	getAutoSet

bool parameter::getAutoOn(void)
{	// getAutoOn
	return Parameter.autoOn;
}	// getAutoOn

bool parameter::getModePPS(void)
{	// getModePPS
	return Parameter.PPS;
}	// getModePPS

controller_c::operating_mode_et parameter::getCVCC(void)
{	// getCVCC
	return (controller_c::operating_mode_et)Parameter.CVCC;
}	// getCVCC

void parameter::getCalU(uint16_t *pCal, uint16_t *pRef)
{	// get CalU
	*pCal=Parameter.CalU;
	*pRef=Parameter.RefU;
}	// get CalU

void parameter::getCalI(uint16_t *pCal, uint16_t *pRef)
{	// get CalI
	*pCal=Parameter.CalI;
	*pRef=Parameter.RefI;
}	// get CalI

uint16_t parameter::getVoltage_mV(void)
{	// getVoltageStep
	return Parameter.setU;
}	// getVoltageStep

uint16_t parameter::getCurrent_mA(void)
{	// getCurrentStep
	return Parameter.setI;
}	// getCurrentStep

/* ************************************************
 * privta funkctions
 * ************************************************
 */

uint16_t parameter::calcParamCnt(void)
{	//	calcParamCnt
	return (Eeprom.getAddressSpace()-DataOffset)/sizeof(parameter_st);
}	//	calcParamCnt

uint16_t parameter::calcParamAddress(uint16_t ParameterPos)
{	//	calcParamAddress
	return DataOffset+ParameterPos*sizeof(parameter_st);
}	//	calcParamAddress

void parameter::nextPos(bool bUp)
{	//	nextPos
	if (bUp)
	{	// use upper half
		lowPos=midPos;
		midPos= (lowPos+highPos+1)>>1;
	}	// use upper half
	else
	{	// use lower half
		highPos=midPos;
		midPos= (lowPos+highPos)>>1;
	}	// use lower half
}	// 	nextPos

void parameter::initPos(void)
{	// initPos
	lowPos=0;
	highPos=calcParamCnt()-1;;
	midPos=0;
	nextPos(true);
}	// initPos


bool parameter::getDirection(void)	// true =up
{	// getDirection
	// !Parameter.dir converts binary data to boolean
	return bDirFlag!=(!Parameter.dir);
}	// getDirection

uint16_t parameter::calcPercentage(uint16_t part, uint16_t total)
{	// try to calculate percentage as exact as possible
	if (part<=655)
	{	// *100 /1
		part*=100;
	}	// *100 /1
	else if (part<=6553)
	{	// *10 /10
		part*=10;
		total/=10;
	}	// *10 /10
	else
	{	// *1 /100
		total/=100;
	}	// *1 /100

	part=part/total;
	if (part>100) part =100;

	return part;
}	// try to calculate percentage as exact as possible

void parameter::setInitData(void)
{	//	setInitData
	Parameter.CalU		=20000;	// Calibration for Output Voltage
	Parameter.RefU		=20000;	// internal measurement for Calibration
	Parameter.CalI		=5000;	// Calibration for output current
	Parameter.RefI		=5000;	// internal measurement for calibration
	Parameter.setU		=5000;	// start value voltage mV
	Parameter.setI		=1000;	// start value current mA
	Parameter.Brightness=50; 	// backlight 50%
	Parameter.PPS		=false;	// slect fix or pps profile
	Parameter.autoSet	=false;	// Flag for automatic setup to last settings on startup
	Parameter.autoOn	=false;	// implies autoSet but switches output on startup
	Parameter.CVCC		=controller_c::CONTROLLER_MODE_OFF;
	Parameter.dir		=false;	// indicator for rotation memory usage
	bDirFlag			=false;	// initial diriction not swapped
}	// 	setInitData

void parameter::process_para_start(void)
{	//	process_para_start
	uint16_t Flag;

	memoryType=NO_MEMORY;
	// check for eeprom
	if (Eeprom.test())
		memoryType=EEPROM;
	// prefere SD Card
	if (hasSD())
		memoryType=SDCARD;

	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			Eeprom.read(flagAddress,&Flag);
			if (flagVal!=Flag)
			{	// incorrect identifier
				paramState=para_clear;
				Address=0;
			}	// incorrect identifier
			else
			{	// correct identifier
				paramState=para_init;
			}	// correct identifier
			break;
		case SDCARD:
			Flag=flagVal;
			if (SD.exists(configFileName))
			{	// file exists, read
				paramState=para_init;
			}	// file exists, read
			else
			{	// file does not exist, create
				paramState=para_write_flag;
			}	// file does not exist, create
			break;
		case NO_MEMORY:
		/* FALLTHRU */
		default:
			Flag=flagVal;
			paramState=para_init;
			break;
	}	// switch memoryTyoe
}	// process_para_start



void parameter::process_para_init(void)
{	// process_para_init
	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			Eeprom.read(calcParamAddress(0),(uint8_t*)&Parameter,sizeof(Parameter));
			bDirFlag=Parameter.dir;
			initPos();
			paramState=para_read;
			break;
		case SDCARD:
			sdState=SD_START;
		/* FALLTHRU */
		case NO_MEMORY:
		/* FALLTHRU */
		default:
			paramState=para_read;
			break;
	}	// switch memoryTyoe

}	// process_para_init

void parameter::process_para_clear(void)
{	// process_para_clear
	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			Eeprom.write_P(Address,(const char *)pgmFF,sizeof(pgmFF));
			Address+=sizeof(pgmFF);
			if ((Address==0) || (Address>=Eeprom.getAddressSpace()))
				paramState=para_write_flag;
			break;
		case SDCARD:
		/* FALLTHRU */
		case NO_MEMORY:
		/* FALLTHRU */
		default:
			paramState=para_write_flag;
			break;
	}	// switch memoryTyoe
}	// process_para_clear

void parameter::process_para_write_flag(void)
{	// process_para_write_flag
	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			Eeprom.write(flagAddress,flagVal);
			paramState=para_write_first;
			break;
		case SDCARD:
			sdState=SD_START;
		/* FALLTHRU */
		case NO_MEMORY:
		/* FALLTHRU */
			paramState=para_write_first;
		default:
			break;
	}	// switch memoryTyoe

}	// process_para_write_flag

void parameter::process_para_write_first(void)
{	// process_para_write_first
	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			Parameter.dir=false;
			bDirFlag=false;
			ParameterPos=0;
			Eeprom.write(calcParamAddress(ParameterPos),(uint8_t*)&Parameter,sizeof(Parameter));
			paramState=para_idle;
			break;
		case SDCARD:
		    if (sdState!=SD_IDLE)
			{	// write config items
				processWriteConfig();
			}	// write config items
			else
			{
				paramState=para_idle;
			}
			break;
		case NO_MEMORY:
		/* FALLTHRU */
			paramState=para_idle;
		default:
			break;
	}	// switch memoryTyoe

}	// process_para_write_first

void parameter::process_para_write(void)
{	// process_para_write
	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			if (ParameterPos>=calcParamCnt()-1)
			{	// end Of memory reached
				// invert direction flag and start from 0
				ParameterPos=0;
				bDirFlag=!bDirFlag;
			}	// end Of memory reached
			else
			{	// stil place to go
				ParameterPos++;
			}	// stil place to go
			Parameter.dir=bDirFlag;
			Eeprom.write(calcParamAddress(ParameterPos),(uint8_t*)&Parameter,sizeof(Parameter));
			paramState=para_idle;
			break;
		case SDCARD:
		    if (sdState!=SD_IDLE)
			{	// write config items
				processWriteConfig();
			}	// write config items
			else
			{
				paramState=para_idle;
			}
			break;
		case NO_MEMORY:
		/* FALLTHRU */
		default:
			paramState=para_idle;
			break;
	}	// switch memoryTyoe

}	// process_para_write


void parameter::process_para_read(void)
{	// process_para_read
	switch (memoryType)
	{	// switch memoryTyoe
		case EEPROM:
			Eeprom.read(calcParamAddress(midPos),(uint8_t*)&Parameter,sizeof(Parameter));
			if (lowPos==highPos)
			{	// last read, ready
				ParameterPos=midPos;
				paramState=para_idle;
			}	// last read, ready
			else if ((midPos==highPos) && (!getDirection()))
			{	// manually set next read
				// this means lowPos is highPos-1
				midPos=lowPos;
				highPos=lowPos;
				// and once more
			}	// manually set next read
			else
			{	// continue search
				nextPos(getDirection());
			}	// continue search
			break;
		case SDCARD:
		    if (sdState!=SD_IDLE)
			{	// write config items
				processReadConfig();
			}	// write config items
			else
			{
				paramState=para_idle;
			}
			break;
		case NO_MEMORY:
		/* FALLTHRU */
		default:
			paramState=para_idle;
			break;
	}	// switch memoryTyoe

}	// process_para_read




const char* parameter::getConfigID(parameter_et param)
{	//getConfigID
	uint8_t n=(uint8_t) param;
	const char * List=configIDList;
	while ((n>0) && (*List!=NUL))
	{	// iterate List
		 List=getNextConfigID(List);
		 n--;
	}	//	iterate List
	return List;
}	//getConfigID

parameter::parameter_et parameter::findConfigID(const char * configLine)
{	// findConfigID
	uint8_t n=0;
	uint8_t m=0;
	const char * configID;
	do
	{	// iterate through config IDs
		m=0;
		configID=getConfigID((parameter_et)n);
		while (configID[m]==configLine[m]) m++;
		if (configID[m]!=NUL) n++;	// no match
	}	// iterate through config IDs
	while ((configID[m]!=NUL) && (n<(uint8_t)ITEM_END));

	return (parameter_et)n;
}	// findConfigID

const char* parameter::getNextConfigID(const char *List)
{	// getNextConfigID
	while (*List) List++;
	List++;
	return List;
}	// getNextConfigID


uint8_t parameter::getConfigIDLen(parameter_et param)
{	// getConfigIDLen
	uint8_t n=0;
	const char * ID=getConfigID(param);
	while (*ID!=NUL)
	{	// get length
		ID++;
		n++;
	}	// get length
	return n;
}	// getConfigIDLen

void parameter::processReadConfig(void)
{	// processReadConfig
	    switch (sdState)
    {   // switch sdState
        case SD_START:
            if (SD.exists(configFileName))
            {   // file exists, OPEN
                sdState=SD_OPEN;
            }   // file exists, OPEM
            else
            {   // file des not exist, exit
                sdState=SD_IDLE;
            }   // file des not exist, exit
            break;
        case SD_OPEN:
            configFile=SD.open(configFileName,FILE_READ);
            parameterIterator=ITEM_CAL_U;
            sdState=SD_DATA;
            break;
        case SD_DATA:
            readConfigLine();
            break;
        case SD_CLOSE:
            configFile.close();
            sdState=SD_IDLE;
            break;
        case SD_IDLE:
            break;
        default:
            sdState=SD_IDLE;
            break;
    }   // switch sdState

}	// processReadConfig

void parameter::readConfigLine(void)
{	// readConfigLine
	parameter_et param;
	uint8_t	 	 cnt=0;
	char 		 data=NUL;
	uint16_t	 val;


	while (configFile.available() && (data!=LF))
	{	// read line
		data=configFile.read();
		configBuffer[cnt]=data;
		cnt=(cnt+1) & (configBufferSize-1);
	}	// read line
	configBuffer[cnt]=NUL;
	
	param=findConfigID(configBuffer);
	if (param!=ITEM_END)
	{	// config ID gefunden
		cnt=getConfigIDLen(param);
		// skip spaces non numbers
		while ( ( (configBuffer[cnt]<'0') ||
				  (configBuffer[cnt]>'9') ) &&
				(configBuffer[cnt]!=NUL) )
			cnt++;
		// convert to val
		val=0;
		while ( (configBuffer[cnt]>='0') &&
				(configBuffer[cnt]<='9') )
		{
			val=val*10;
			val=val+configBuffer[cnt]-'0';
			cnt++;
		}
	}	// config ID gefunden
	
	switch (param)
	{	// switch param
		case ITEM_CAL_U:
			Parameter.CalU=val;
			break;
		case ITEM_REF_U:
			Parameter.RefU=val;
			break;
		case ITEM_CAL_I:
			Parameter.CalI=val;
			break;
		case ITEM_REF_I:
			Parameter.RefI=val;
			break;
		case ITEM_SET_U:
			Parameter.setU=val;
			break;
		case ITEM_SET_I:
			Parameter.setI=val;
			break;
		case ITEM_BRIGHTNESS:
			Parameter.Brightness=val;
			break;
		case ITEM_PPS:
			Parameter.PPS=val;
			break;
		case ITEM_AUTO_SET:
			Parameter.autoSet=val;
			break;
		case ITEM_AUTO_ON:
			Parameter.autoOn=val;
			break;
		case ITEM_REGULATOR:
			Parameter.CVCC=val;
			break;
		case ITEM_END:
			/* FALLTHRU */
		default:
			break;
	}	// switch param

	if (configFile.available()==0) 
	{	// file parsed
		sdState=SD_CLOSE;
		parameterIterator=ITEM_END;
	}	// file parsed
}	// readConfigLine

void parameter::processWriteConfig(void)
{   //  processWriteConfig

    switch (sdState)
    {   // switch sdState
        case SD_START:
            if (SD.exists(configFileName))
            {   // file exists, remove first
                sdState=SD_DELETE;
            }   // file exists, remove first
            else
            {   // file des not exist, open
                sdState=SD_OPEN;
            }   // file des not exist, open
            break;
        case SD_DELETE:
            SD.remove(configFileName);
            sdState=SD_OPEN;
            break;
        case SD_OPEN:
            configFile=SD.open(configFileName,FILE_WRITE);
            parameterIterator=ITEM_CAL_U;
            sdState=SD_DATA;
            break;
        case SD_DATA:
            writeConfigLine();
            break;
        case SD_CLOSE:
            configFile.close();
            sdState=SD_IDLE;
            break;
        case SD_IDLE:
            break;
        default:
            sdState=SD_IDLE;
            break;
    }   // switch sdState

}   //  processWriteConfig

void parameter::writeConfigLine(void)
{	// writeConfigLine

	configFile.print(getConfigID(parameterIterator));

	switch (parameterIterator)
	{	// switch param
		case ITEM_CAL_U:
			configFile.println(Parameter.CalU);
			break;
		case ITEM_REF_U:
			configFile.println(Parameter.RefU);
			break;
		case ITEM_CAL_I:
			configFile.println(Parameter.CalI);
			break;
		case ITEM_REF_I:
			configFile.println(Parameter.RefI);
			break;
		case ITEM_SET_U:
			configFile.println(Parameter.setU);
			break;
		case ITEM_SET_I:
			configFile.println(Parameter.setI);
			break;
		case ITEM_BRIGHTNESS:
			configFile.println(Parameter.Brightness);
			break;
		case ITEM_PPS:
			configFile.println(Parameter.PPS);
			break;
		case ITEM_AUTO_SET:
			configFile.println(Parameter.autoSet);
			break;
		case ITEM_AUTO_ON:
			configFile.println(Parameter.autoOn);
			break;
		case ITEM_REGULATOR:
			configFile.println(Parameter.CVCC);
			break;
		case ITEM_END:
			/* FALLTHRU */
			sdState=SD_CLOSE;
		default:
			break;
	}	// switch param
	if (parameterIterator<ITEM_END)
        parameterIterator=(parameter_et)((uint8_t)parameterIterator+1);
}	// writeConfigLine

#undef PRITNLN
#undef PRITN
#undef PRITNLN_HEX
#undef PRITN_HEX
