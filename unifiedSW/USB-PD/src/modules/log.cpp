#include "log.h"
#include <SD.h>
#include "../ASCII/ASCII_ctrl.h"
#include "../tool/convert.h"
#include "../memory/sd_detect.h"

const char log_c::logIntervalText[]=		" off  \0"
											"250 ms\0"
											"500 ms\0"
											"  1 s \0"
											"2.5 s \0"
											"  5 s \0"
											" 10 s \0"
											" 15 s \0"
											" 20 s \0"
											" 30 s \0\0";
const uint16_t log_c::logInterval_ms[]= 	{    0,   250,   500,  1000, 
												 2500,  5000, 10000, 15000, 
												15000, 20000, 30000,     0 };
const char log_c::logTypeExtension[]=	"LOG";

log_c::log_c(void)
{
	logState=INIT;
	pController=NULL;
}

void log_c::init(controller_c * controller)
{
	pController=controller;
}


bool log_c::process(void)
{
	logTime=millis();
	switch (logState)
	{	// switch logState
		case INIT:
			logFileNum=0;
			logDuration=0;
			setInterval(LOG_OFF);
			dir.set_extension_filter(logTypeExtension);
			if (hasSD())
				logState=DIR_SEARCH;
			else
				logState=NO_CARD;
			break;
		case DIR_SEARCH:
			if (dir.next_file())
			{	// parse dir
				if (dir.is_match())
				{
					uint32_t number=convert_c(dir.get_file_name()).getUnsigned(0);
					if (number>logFileNum) logFileNum=number;
				}
			}	// parse dir
			else
			{	// finished
				logState=IDLE;
				logFileNum++;
			}	// finished
			break;
		case IDLE:
			if ( (logTimer>0) && (logTime>=nextLog) )
			{	// check if it's log time
				if (hasSD())
				{	// card available
					log();
					nextLog+=logTimer;
				}	// card available
				else
				{	// no card
					logState=NO_CARD;
					logFileNum=0;
				}	// no card
			}	// check if it's log time
			break;
		case NO_CARD:
			// do nothing 
			break;
		default:
			logState=INIT;
			break;
	}	// switch logState
	return isBusy();
	
}

bool log_c::isBusy(void)
{	// isBusy
	return logState<IDLE;
}	// isBusy

bool log_c::hasSD(void)
{	// wrapper for sd_detect
	return sd_detect.present();
}	// wrapper for sd_detect


const char * log_c::getLogIntervalText(log_c::logInterval_et interval)
{	// getLogIntervalText
	uint8_t cnt=(uint8_t)interval;
	const char * text=logIntervalText;
	while ((cnt>0) && (*text!=NUL))
	{	// search text
		while (*text++!=NUL); // next item
		cnt--;
	}	// search text
	return text;
}	// getLogIntervalText

log_c::logInterval_et log_c::nextInterval(log_c::logInterval_et interval)
{	// nextInterval
	if (((uint8_t)interval)<(((uint8_t)LOG_NN)-1))
		return (logInterval_et(((uint8_t)interval)+1));
	else
		return interval;
}	// nextInterval

log_c::logInterval_et log_c::previousInterval(log_c::logInterval_et interval)
{	// previousInterval
	if (((uint8_t)interval)>((uint8_t)LOG_OFF))
		return (logInterval_et(((uint8_t)interval)-1));
	else
		return interval;
}	// previousInterval


void log_c::setInterval(log_c::logInterval_et interval)
{	//	setInterval
	if (logState!=NO_CARD)
	{ 	// SD CARD present, continue
		if ((logInterval==LOG_OFF) && (interval!=LOG_OFF))
		{	// open file
			char name[13];
			makeName(name);
			LOG=SD.open(name,FILE_WRITE);
			LOG.print("---- time -----;mod;-U/V-;-I/V-;Ubus/V;Uout/V;Iout/A;");
			if (pController->has_temperature())
				LOG.print("deg C;");
			LOG.println("ms");
			LOG.flush();
		}	// open file
		if ((logInterval!=LOG_OFF) && (interval==LOG_OFF))
		{	// close file
			LOG.close();
			logFileNum++;
		}	// close file
		
		logInterval=interval;
		logTimer=logInterval_ms[(uint8_t)interval];
		nextLog=logTime-logTime%logTimer;
	} 	// SD CARD present, continue

}	//	setInterval







/****************************************
 * private functions
 ****************************************/

void log_c::log(void)
{	// log
	char buffer[20];
	uint32_t start=millis();
	
	// log Time
	LOG.print(convert_c(logTime,3).getStringTime(buffer,true,3));
	LOG.print(";");
	
	// log mode
	if (pController->is_PPS())
	{	// PPS
		switch (pController->get_operating_mode())
		{	// switch operating mode
			case controller_c::CONTROLLER_MODE_CV:
				LOG.print("U  ;");
				break;
			case controller_c::CONTROLLER_MODE_CVCC:
				LOG.print("UI ;");
				break;
			case controller_c::CONTROLLER_MODE_CVCCmax:
				LOG.print("UI^;");
				break;
			case controller_c::CONTROLLER_MODE_OFF:
			default:
				LOG.print("PPS;");
				break;
		}	// switch operating mode
	}	// PPS
	else
	{	// FIX
		LOG.print("FIX;");
	}	// FIX
	
	// log Uset
	LOG.print(convert_c(pController->get_set_voltage(),3).getStringUnsigned(buffer,5,2,";"));
	// log Iset
	LOG.print(convert_c(pController->get_set_current(),3).getStringUnsigned(buffer,5,2,";"));
	// log Ubus
	LOG.print(convert_c(pController->get_Vbus_mV(),3).getStringUnsigned(buffer,6,3,";"));
	// log Uout
	LOG.print(convert_c(pController->get_output_voltage_mV(),3).getStringUnsigned(buffer,6,3,";"));
	// log Iout
	LOG.print(convert_c(pController->get_output_current_mA(),3).getStringUnsigned(buffer,6,3,";"));

	// log temperatur
	if (pController->has_temperature())
		LOG.print(convert_c(pController->get_temperature_dC(),1).getStringSigned(buffer,5,1,";"));
	/* debug */
	LOG.print(convert_c(logDuration,0).getStringUnsigned(buffer,2,0));
	LOG.println();

	LOG.flush();
	logDuration=millis()-start;
}	// log



void log_c::makeName(char * buffer)
{	// create log file name
	const char *pExt=logTypeExtension;
	// clear name
	for (uint8_t n=0; n<13; buffer[n++]=NUL);
	// file name
	convert_c(logFileNum,0).getStringUnsignedZeros(buffer,8,0);
	// add dot
	buffer=&buffer[8];
	*buffer++='.';
	// add extension
	while ( (*buffer++ = *pExt++) );
}	// create log file name
