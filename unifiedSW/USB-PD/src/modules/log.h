#ifndef __log_h__
#define __log_h__

#include <stdint.h>
#include <SD.h>
#include "controller.h"
#include "../memory/dir.h"

class log_c 
{
	public:
		typedef enum:uint8_t { 	LOG_OFF, 
								LOG_250ms, LOG_500ms, LOG_1000ms, 
								LOG_2500ms, LOG_5000ms, LOG_10000ms,
								LOG_15000ms, LOG_20000ms, LOG_30000ms, 
								LOG_NN } logInterval_et;

		log_c(void);
		
		void init(controller_c * controller);
		
		bool process(void);
		
		bool isBusy(void);

		bool hasSD(void);
		
		const char * getLogIntervalText(logInterval_et interval);
		const char * getLogIntervalTextList(void) {return logIntervalText; } 

		logInterval_et nextInterval(logInterval_et interval);
		logInterval_et previousInterval(logInterval_et interval);
		logInterval_et getInterval(void) { return logInterval; }
		void setInterval(logInterval_et interval);
		
		uint32_t getLogFileNumber(void) { return logFileNum; }
		uint16_t getLogInterval_ms(logInterval_et interval) { return logInterval_ms[(uint8_t)interval];} 
		
	private:
		typedef enum:uint8_t {INIT, DIR_SEARCH, IDLE, NO_CARD } logState_et;
		
		static const char logIntervalText[];
		static const uint16_t logInterval_ms[];
		static const char logTypeExtension[];
		
		logState_et logState;
		logInterval_et logInterval;
		uint32_t logFileNum;
		uint32_t logTime;
		uint32_t nextLog;
		uint16_t logTimer;
		uint32_t logDuration;

		File	LOG;
		dir_c	dir;
		
		controller_c * pController;


		void log(void);
		
		void makeName(char * buffer); 
};

#endif // __log_h_
