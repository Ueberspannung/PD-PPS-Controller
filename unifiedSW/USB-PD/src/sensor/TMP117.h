#ifndef _TMP117_
#define _TMP117_

#include <stdint.h>
#include <stdbool.h>

class TMP117
{
	public:
	
		typedef enum:uint8_t {	ALERT_PIN_ACTIVE_LOW,
								ALERT_PIN_ACTIVE_HIGH 
								} alert_pin_polarity_et;
		
		typedef enum:uint8_t {	ALERT_PIN_ALERT,
								ALERT_PIN_DATA_READY 
								} alert_pin_select_et;

		typedef enum:uint8_t {	ALERT_MODE_THERMISTOR,		// comparator with hysteresis 
								ALERT_MODE_ALERT            // interrupt on over / under limit	
								} alert_mode_select_et;

		typedef enum:uint8_t {	AVERAGING_OFF,
								AVERAGING_8,
								AVERAGING_32,
								AVERAGING_64
								} averaging_mode_et;
		
		typedef enum:uint8_t {	CONVERSION_TIME_1_64s,
								CONVERSION_TIME_1_8s,
								CONVERSION_TIME_1_4s,
								CONVERSION_TIME_1_2s,
								CONVERSION_TIME_1s,
								CONVERSION_TIME_4s,
								CONVERSION_TIME_8s,
								CONVERSION_TIME_16s
								} conversion_time_et;
								
		typedef enum:uint8_t {	MODE_CONTINUOUS,
								MODE_SHUTDOWN,
								MODE_CONTINUOUS_RES,
								MODE_ONE_SHOT
								} conversion_mode_et;
		
		typedef enum:uint8_t {	EEPROM_LOCK,
								EEPROM_UNLOCK
								} eeprom_lock_mode_et;
		
		typedef enum:uint8_t {  EEPROM_POS_1,
								EEPROM_POS_2,
								TEMPERATURE_OFFSET,
								EEPROM_POS_3 
								} eeprom_pos_et;
	
	
		TMP117(uint8_t addr, uint8_t alert_pin=-1);		// Constructor with i2c address and int pin
		
		bool init(void);
			
		bool process_idle(void);	// non blocking processing, returns true if idle
		
		bool isAlert(void);				// test Alert Pin
		
		int16_t getTemp(uint8_t decimals);	// gets temperature in decimal fixed point notation
		int16_t getTemp(void);				// gets temperature in binary s8.7 fixed point notiation
		
		void setHighTemperaturLimit(int16_t tempDec, uint8_t decimals);	// set value in decimal fixed point notation
		void setHighTemperaturLimit(int16_t tempIQ);					// set value in binary s8.7 fixed point notation
		void setLowTemperaturLimit(int16_t tempDec, uint8_t decimals);	// set value in decimal fixed point notation
		void setLowTemperaturLimit(int16_t tempIQ);						// set value in binary s8.7 fixed point notation

		int16_t getHighTemperaturLimit(uint8_t decimals);	// get value in decimal fixed point notation
		int16_t getHighTemperaturLimit(void);				// get value in binary s8.7 fixed point notation 						
		int16_t getLowTemperaturLimit(uint8_t decimals);	// get value in decimal fixed point notation
		int16_t getLowTemperaturLimit(void);                // get value in binary s8.7 fixed point notation
		
		uint16_t getDeviceID(void);
		uint8_t getDeviceRevision(void);
		
		void reset(void);
		
		void setAlertPinSource(alert_pin_select_et source);
		void setAlertPinPolarity(alert_pin_polarity_et polarity);
		void setAlertMode(alert_mode_select_et mode);
		void setAveragingMode(averaging_mode_et mode);
		void setConversionTime(conversion_time_et time);
		void setConversionMode(conversion_mode_et mode);
		
		
		alert_mode_select_et 	getAlertPinSource(void);
		alert_pin_polarity_et 	getAlertPinPolarity(void);
		alert_mode_select_et 	getAlertMode(void);
		averaging_mode_et		getAveragingMode(void);
		conversion_time_et		getConversionTime(void);
		conversion_mode_et		getConversionMode(void);

		bool isDataReady(void);
		bool testHighTemperatureAlert(void);
		bool testLowTemperatureAlert(void);
		bool testEepromBusy(void);
		bool testEepromLocked(void);
	
		void setEepromLockState(eeprom_lock_mode_et lock);
		void writeEeprom(eeprom_pos_et Register, uint16_t val);
		void writeTemperatureOffset(int16_t val);
		
		uint16_t readEeprom(eeprom_pos_et Register);
		int16_t readTemperatureOffset(void);
		
		
	private:

		static const uint8_t	REG_TEMPERATURE		=0;
		static const uint8_t	REG_CONFIGURATION	=1;
		static const uint8_t	REG_HIGH_TEMP_LIMIT	=2;
		static const uint8_t	REG_LOW_TEMP_LIMIT	=3;
		static const uint8_t	REG_EEPROM_UNLOCK	=4;
		static const uint8_t	REG_EEPROM_1		=5;
		static const uint8_t	REG_EEPROM_2		=6;
		static const uint8_t	REG_TEMP_OFFSET		=7;
		static const uint8_t	REG_EEPROM_3		=8;
		static const uint8_t	REG_DEVICE_ID		=15;
		
		static const uint8_t	EEPROM_WRITE_DELAY	=7;

		typedef enum:uint8_t {	EEPROM_IDLE,
								EEPROM_WRITE_WRITE,
								EEPROM_WRITE_WAIT,
								EEPROM_WRITE_BUSY,
								EEPROM_START
							}	eeprom_process_et;

		typedef struct configReg_s { 	uint16_t reserved:1;
										uint16_t SoftReset:1;			// 1=Reset
										uint16_t AlertPinSelect:1;		// 0=Alert, 1=Date Ready
										uint16_t AlertPinPolarity:1;	// 1=active high, 0=active low
										uint16_t AlertMode:1;			// 1=Thermistor, 0=Alert
										uint16_t AveragingMode:2;		// 00=off, 01=8, 10=32, 11=64
										uint16_t ConversionCycleTime:3;	// 000: off no delay, 64/s
																		// 001:  125ms,	8/s
																		// 010:  500ms,	2/s
																		// 011: 1000ms,	1/s
																		// 100:	2000ms,	1/2s
																		// 101: 4000ms,	1/4s
																		// 110: 8000ms,	1/8s
																		// 111:16000ms,	1/16s
										uint16_t ConversionMode:2;		// 00: Continuous conversion (CC)
																		// 01: Shutdown (SD)
																		// 10: Continuous conversion (CC), Same as 00 (reads back = 00)
																		// 11: One-shot conversion (OS)
										uint16_t EEPROM_Busy:1;			// 1= write operation in progress
										uint16_t DataReady:1;			// 1= new temperature value in temperature register
										uint16_t lowTempAlert:1;		// 1= new temperature is below low limit
										uint16_t highTempAlert:1;		// 1= new temperature is above high limit
									}	configReg_st;
									
		typedef union configReg_u	{	uint16_t 		RegisterData;
										configReg_st 	Flags;
									}	configReg_ut;
									
		typedef struct EEPROM_UnlockReg_s {	uint16_t reserved:14;		// reserved
											uint16_t EUN:1;				// 1=unlocked, write to EEPROM, 
																		// 0=locked, write to temp. register
											uint16_t EEPROM_Busy:1;		// 1= write operation in progress
										}	EEPROM_UnlockReg_st;
										
		typedef union EEPROM_UnlockReg_u {	uint16_t RegisterData;
											EEPROM_UnlockReg_st Flags;
										 }	EEPROM_UnlockReg_ut;
										 
		typedef struct deviceID_s 	{	uint16_t ID:12;					// Device ID (0x117)
										uint16_t Rev:4;					// Device Revision (currently 0) 
									}	deviceID_st;
		
		typedef union deviceID_u	{	uint16_t RegisterData;
										deviceID_st Flags;
									}	deviceID_ut;

		eeprom_process_et	eeprom_state;
		configReg_ut		configReg;

		uint8_t 			i2c_address;
		uint16_t			time;
		uint8_t				int_pin;
		bool				int_pin_active_high;


		int16_t convertToIQ(int16_t valDec, uint8_t decimals);	// converts a value given as
																// integer part of temperature*10^decimals
																// to IQ 9.7 value
																// 9.5°C given as (95,1) yields in 0x04C0	
		int16_t convertToDec(int16_t valIQ, uint8_t decimals);	// converts a value given as IQ 9.7 value to
																// the integer part of temperature*10^decimals
																// 9,5°C given as IQ 9.7 equals 0x04C0 
																// (0x04C0,1) yields in 95	 
		
		uint16_t read_word(uint8_t reg);
		void write_word(uint8_t reg, uint16_t val);
};

#endif // _TMP117_
