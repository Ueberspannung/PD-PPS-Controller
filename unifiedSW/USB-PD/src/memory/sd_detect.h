#ifndef __sddetect_h__
#define __sddetect_h__

#include <SD.h>
#include "../../config.h"

class sd_detect_c
{
	public:
	
		bool present(void);
		
	private:
										
		typedef enum:uint8_t { 	SDCARD_START,
								SDCARD_ONLINE,
								SDCARD_OFFLINE } sd_state_et;

		static sd_state_et sd_state;
};

#endif // __sddetect_h__
