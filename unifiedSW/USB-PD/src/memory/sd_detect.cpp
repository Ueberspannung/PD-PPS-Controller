#include "sd_detect.h"
#include <SD.h>
#include "../../config.h"

sd_detect_c::sd_state_et sd_detect_c::sd_state=sd_detect_c::SDCARD_START;
uint32_t sd_detect_c::sd_time;

bool sd_detect_c::present(void)
{	// detect SD-Card and init
	switch (sd_state)
	{	// switch state
		case SDCARD_START:
			#ifndef PD_PPS_CONTROLLER
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
			#else
				sd_state=SDCARD_OFFLINE;
			#endif
			break;
		case SDCARD_ONLINE:
			#ifndef PD_PPS_CONTROLLER
				// check if card has been removed
				if (digitalRead(HWCFG_SDCARD_CD))
					sd_state=SDCARD_OFFLINE;
			#else
				sd_state=SDCARD_OFFLINE;
			#endif
			break;
		case SDCARD_OFFLINE:
			#ifndef PD_PPS_CONTROLLER
				// check if card has been removed
				if (!digitalRead(HWCFG_SDCARD_CD))
				{	// sd-card inserted?
					sd_state=SDCARD_WAIT;
					sd_time=millis();
				}	// sd-card inserted?
			#endif
			// final fail
			break;
		case SDCARD_WAIT:
			#ifndef PD_PPS_CONTROLLER
				// check if card has been removed
				if (!digitalRead(HWCFG_SDCARD_CD))
				{	// sd-card inserted?
					if (millis()-sd_time>sd_delay)
						sd_state=SDCARD_INSERTED;
				}	// sd-card inserted?
				else
					sd_state=SDCARD_OFFLINE;
			#else
				sd_state=SDCARD_OFFLINE;
			#endif
			break;
		case SDCARD_INSERTED:
			#ifndef PD_PPS_CONTROLLER
				// check if card has been removed
				if (digitalRead(HWCFG_SDCARD_CD))
				{	// sd-card inserted?
					sd_state=SDCARD_OFFLINE;
				}	// sd-card inserted?
			#else
				sd_state=SDCARD_OFFLINE;
			#endif
			break;
		default:
			break;
	}	// switch state
	return sd_state==SDCARD_ONLINE;
}	// detect SD-Card and init

bool sd_detect_c::inserted(void)
{	// check if sd card has been inserted
	return sd_state==SDCARD_INSERTED;
}	// check if sd card has been inserted


sd_detect_c sd_detect;
