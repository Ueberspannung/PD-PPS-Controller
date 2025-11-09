#ifndef __sddetect_h__
#define __sddetect_h__

#include <SD.h>
#include <stdint.h>
#include "../../config.h"

class sd_detect_c
{
	public:
	
		bool present(void);
		bool inserted(void);
		
	private:
										
		typedef enum:uint8_t { 	SDCARD_START,
								SDCARD_ONLINE,
								SDCARD_OFFLINE,
								SDCARD_WAIT,
								SDCARD_INSERTED } sd_state_et;

		static const uint16_t sd_delay=2000;
		static sd_state_et sd_state;
		static uint32_t	sd_time;
};

extern sd_detect_c sd_detect;

#endif // __sddetect_h__
