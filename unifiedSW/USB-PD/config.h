#pragma once

#ifndef __config_h__
#define __config_h__


//#define PD_PPS_CONTROLLER			// definitions for pdMicro rev. 1
//#define PD_PPS_CONTROLLER_2		// definitions for pdMicro rev. 2 
#define PD_PPS_CONTROLLER_MINI 	// definitions for pdMicroMin rev. 1



#if defined(PD_PPS_CONTROLLER)


	#define HWCFG_POWER_SWITCH 		(2)
	#define HWCFG_LED_GREEN			(3)
	#define HWCFG_LED_RED			(5)
	#define HWCFG_LED_BLUE			(4)
	#define HWCFG_LED_OFF			(true)
	
	#define HWCFG_FUSB302INT_PIN	(7)
	#define HWCFG_IMU_INT_PIN		(14)	// just for completeness, not present in this rev.
	#define HWCFG_ADC_INT_PIN		(24)	// just for completeness, not present in this rev.
	#define HWCFG_TMP_INT_PIN		(0xFF)	//(30)	// just for completeness, not present in this rev.

	#define HWCFG_CAT4004_PIN		(8)		// alt. 40 (identical port)
	#define HWCFG_CAT4004_TYPE		('A')
	#define HWCFG_LCD_PWM_PIN		(0xFF)	// no PWM pin used

	// SPI Display interface not present in this HW
	#define HWCFG_DISPLAY_RESET		(6)
	#define HWCFG_DISPLAY_CS		(10)
	#define HWCFG_DISPLAY_MOSI		(11)
	#define HWCFG_DISPLAY_DC		(12)	// is miso pin

	#define HWCFG_LED_ORANGE		(13)
	#define HWCFG_ENCODER_SWITCH	(14)
	#define HWCFG_ENCODER_CLOCK		(17)
	#define HWCFG_ENCODER_DATA		(18)
	#define HWCFG_SWITCH_OK_SELECT	(15)	// just for completeness, not present in this rev.
	#define HWCFG_SWITCH_UP			(16)	// just for completeness, not present in this rev.
	#define HWCFG_SWITCH_DOWN		(25)	// just for completeness, not present in this rev.
	#define HWCFG_VBUS_ANALOG_PIN	(19)	// A5 defaults to pin#19

	#define HWCFG_FUSB302_I2C_ADDR	(0x22)	// hard set in PD_UFP.cpp #82
	#define HWCFG_LCD_I2C_ADDR		(0x3F)
	#define HWCFG_ADC_I2C_ADDR		(0x40)
	#define HWCFG_ADC_TYPE_INA219	// just a macro to indicate adc type
	#define HWCFG_EEPROM_I2C_ADDR	(0x50)
	#define HWCFG_EEPROM_DENSITY	(eeprom::eeprom_32kbit)

	#define HWCFG_TMP_I2C_ADDR		(0x48)
	#define HWCFG_TMP_TYPE_TMP117	// just a macro to indicate temperature sensor type

#elif defined(PD_PPS_CONTROLLER_2)

#elif defined(PD_PPS_CONTROLLER_MINI)

	#define HWCFG_POWER_SWITCH 		(23)
	#define HWCFG_LED_GREEN			(17)
	#define HWCFG_LED_RED			(31)
	#define HWCFG_LED_BLUE			(18)
	#define HWCFG_LED_OFF			(true)

	#define HWCFG_FUSB302INT_PIN	(7)
	#define HWCFG_IMU_INT_PIN		(14)
	#define HWCFG_ADC_INT_PIN		(24)
	#define HWCFG_TMP_INT_PIN		(30)
	
	#define HWCFG_CAT4004_PIN		(8)		// alt. 40 (identical port)
	#define HWCFG_CAT4004_TYPE		('A')
	#define HWCFG_LCD_PWM_PIN		(0xFF)	// no PWM pin used
	#define HWCFG_DISPLAY_RESET		(6)
	#define HWCFG_DISPLAY_CS		(10)
	#define HWCFG_DISPLAY_MOSI		(11)
	#define HWCFG_DISPLAY_DC		(12)	// is MISO pin
	#define HWCFG_DISPLAY_SCK		(13)
	#define HWCFG_LED_ORANGE		(13)	// just for completeness
	#define HWCFG_ENCODER_SWITCH	(14)
	#define HWCFG_ENCODER_CLOCK		(17)
	#define HWCFG_ENCODER_DATA		(18)
	#define HWCFG_SWITCH_OK_SELECT	(15)
	#define HWCFG_SWITCH_UP			(16)
	#define HWCFG_SWITCH_DOWN		(25)	// attention RX Led on Zero needs to be undefined in variants.txt

	#define HWCFG_VBUS_ANALOG_PIN	(19)	// A5 defaults to pin#19

	#define HWCFG_FUSB302_I2C_ADDR	(0x22)	// hard set in PD_UFP.cpp #82
	#define HWCFG_LCD_I2C_ADDR		(0x3F)	// dos not exist
	#define HWCFG_ADC_I2C_ADDR		(0x40)	
	//#define HWCFG_ADC_TYPE_INA219	// just a macro to indicate adc type, if undef INA232 is used
	#define HWCFG_EEPROM_I2C_ADDR	(0x50)	// does not exist Parameter in fake mode
	#define HWCFG_EEPROM_DENSITY	(eeprom::eeprom_32kbit)
	#define HWCFG_TMP_I2C_ADDR		(0x48)

	
	#define HWCFG_SDCARD_MOSI		(22)	// PA12
	#define HWCFG_SDCARD_MISO		(5)		// PA15
	#define HWCFG_SDCARD_SCK		(38)	// PA13
	#define HWCFG_SDCARD_SS			(2)		// PA14
	#define HWCFG_SDCARD_CD			(26)	// PA27
	
#else
	#error no hardware defined
#endif  

#if defined(PD_PPS_CONTROLLER) && defined(PD_PPS_CONTROLLER_2) ||\
	defined(PD_PPS_CONTROLLER_2) && defined(PD_PPS_CONTROLLER_MINI) ||\
	defined(PD_PPS_CONTROLLER) && defined(PD_PPS_CONTROLLER_MINI) 

	#error multiple hardware defined

#endif

#endif //__config_h__
