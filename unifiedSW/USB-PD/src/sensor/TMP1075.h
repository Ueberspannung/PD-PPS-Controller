#ifndef _TMP1075_
#define _TMP1075_

#include <stdint.h>
#include <stdbool.h>

class TMP1075_c
{
	public:
	
		typedef enum:uint8_t {	ALERT_PIN_ACTIVE_LOW,
								ALERT_PIN_ACTIVE_HIGH 
								} alert_pin_polarity_et;

		typedef enum:uint8_t {	ALERT_MODE_COMPARATOR, 		// comparator with hysteresis 
								ALERT_MODE_INTERRUPT		// interrupt on over / under limit	
								} alert_mode_select_et;

		typedef enum:uint8_t {	FAULT_COUNT_1,
								FAULT_COUNT_2,
								FAULT_COUNT_3,
								FAULT_COUNT_4
								} fault_count_et;
		
		typedef enum:uint8_t {	CONVERSION_TIME_1_36s,
								CONVERSION_TIME_1_18s,
								CONVERSION_TIME_1_9s,
								CONVERSION_TIME_1_4s
								} conversion_time_et;
								
		typedef enum:uint8_t {	MODE_CONTINUOUS,
								MODE_SHUTDOWN,
								MODE_ONE_SHOT
								} conversion_mode_et;
		
			
		TMP1075_c(uint8_t addr, uint8_t alert_pin=-1);		// Constructor with i2c address		

		bool init(void);				// test if chip exists and set int pin

		bool isAlert(void);				// test Alert Pin
		
		int16_t getTemp(uint8_t decimals);	// gets temperature in decimal fixed point notation
		int16_t getTemp(void);				// gets temperature in binary s7.8 fixed point notiation
		
		void setHighTemperaturLimit(int16_t tempDec, uint8_t decimals);	// set value in decimal fixed point notation
		void setHighTemperaturLimit(int16_t tempIQ);					// set value in binary s7.8 fixed point notation
		void setLowTemperaturLimit(int16_t tempDec, uint8_t decimals);	// set value in decimal fixed point notation
		void setLowTemperaturLimit(int16_t tempIQ);						// set value in binary s7.8 fixed point notation

		int16_t getHighTemperaturLimit(uint8_t decimals);	// get value in decimal fixed point notation
		int16_t getHighTemperaturLimit(void);				// get value in binary s7.8 fixed point notation 						
		int16_t getLowTemperaturLimit(uint8_t decimals);	// get value in decimal fixed point notation
		int16_t getLowTemperaturLimit(void);                // get value in binary s7.8 fixed point notation
		
		uint16_t getDeviceID(void);							// not available on TMP175N
		
		void reset(void);
		
		void setAlertPinPolarity(alert_pin_polarity_et polarity);
		void setAlertMode(alert_mode_select_et mode);
		void setFaultCount(fault_count_et count);
		void setConversionTime(conversion_time_et time);
		void setConversionMode(conversion_mode_et mode);
		
		
		alert_pin_polarity_et 	getAlertPinPolarity(void);
		alert_mode_select_et 	getAlertMode(void);
		fault_count_et			getFaultCount(void);
		conversion_time_et		getConversionTime(void);
		conversion_mode_et		getConversionMode(void);

		bool isDataReady(void);	// only on TMP1075N in one shot mode
		bool isTMP1075N(void);	// ceck for N revision
	private:

		static const uint8_t	REG_TEMPERATURE		=0;
		static const uint8_t	REG_CONFIGURATION	=1;
		static const uint8_t	REG_LOW_TEMP_LIMIT	=2;
		static const uint8_t	REG_HIGH_TEMP_LIMIT	=3;
		static const uint8_t	REG_DEVICE_ID		=15;
		
	
		typedef struct configReg_s { 	uint16_t reserved:8;
										uint16_t ShutDown:1;			// 1=ShutDown Mode
										uint16_t AlertMode:1;			// 1=Thermistor, 0=Alert
										uint16_t AlertPinPolarity:1;	// 1=active high, 0=active low
										uint16_t FaultCount:2;			// 0-3  1-4 faults
										uint16_t ConversionCycleTime:2;	// 00:  27,5ms, 64/s
																		// 01:  55ms,	8/s
																		// 10:  110ms,	2/s
																		// 11:  220ms,	1/s
										uint16_t OneShotMode:1;			// 1= singel conversion
									}	configReg_st;
		
		typedef struct word_s		{	uint8_t lo;
										uint8_t hi;
									}	word_st;
								
		typedef union configReg_u	{	uint16_t 		RegisterData;
										word_st			RegisterWord;
										configReg_st 	Flags;
									}	configReg_ut;
		typedef enum:uint8_t		{	DETECTION_IDLE,
										DETECTED_TMP1075,
										DETECTED_TMP1075N
									} tmp1075_detect_et;
										 		
		configReg_ut		configReg;

		tmp1075_detect_et	tmp1075_type;
	
		uint8_t 			i2c_address;
		uint8_t				int_pin;
		bool				int_pin_active_high;
		

		int16_t convertToIQ(int16_t valDec, uint8_t decimals);	// converts a value given as
																// integer part of temperature*10^decimals
																// to IQ 8.8 value
																// 9.5°C given as (95,1) yields in 0x0980	
		int16_t convertToDec(int16_t valIQ, uint8_t decimals);	// converts a value given as IQ 8.8 value to
																// the integer part of temperature*10^decimals
																// 9,5°C given as IQ 8.8 equals 0x0980 
																// (0x0980,1) yields in 95	 
		
		uint16_t read_word(uint8_t reg);
		void write_word(uint8_t reg, uint16_t val);
		void write_byte(uint8_t reg, uint8_t val);
};

#endif // _TMP1075_
