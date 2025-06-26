#include "sd_detect.h"
#include <SD.h>
#include "../../config.h"

sd_detect_c::sd_state_et sd_detect_c::sd_state=sd_detect_c::SDCARD_START;

bool sd_detect_c::present(void)
{	// detect SD-Card and init
	switch (sd_state)
	{	// switch state
		case SDCARD_START:
			pinMode(HWCFG_SDCARD_CD,INPUT_PULLUP);
			if (!digitalRead(HWCFG_SDCARD_CD))
			{	// card inserted
				if (SD.begin())
				{	// init ok
					sd_state=SDCARD_ONLINE;
				}	// init ok
				else
				{	// init fail
					sd_state=SDCARD_OFFLINE;
				}	// init fail
			}	// card inserted
			else
			{	// no card inserted
				sd_state=SDCARD_OFFLINE;
			}	// no card inserted
			break;
		case SDCARD_ONLINE:
			// check if card has been removed
			if (digitalRead(HWCFG_SDCARD_CD))
				sd_state=SDCARD_OFFLINE;
			break;
		case SDCARD_OFFLINE:
			// final fail
			break;
		default:
			break;
	}	// switch state
	return sd_state==SDCARD_ONLINE;
}	// detect SD-Card and init
