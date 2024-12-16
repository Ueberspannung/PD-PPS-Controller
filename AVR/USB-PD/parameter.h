#ifndef _parameter_h_
#define _parameter_h_
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "eeprom.h"

class parameter
{	
	public:
		typedef enum:uint8_t { 	regulator_mode_off, 
								regulator_mode_cv, 
								regulator_mode_cvcc, 
								regulator_mode_cvcc_max } regulator_mode_et;
	
		parameter();
		
		bool process(void);		// returns true if busy
		uint8_t getProgress(void);
		void read(void);
		void write(void);
		void clear(bool bSetInitData);

		void setBrightness(uint8_t brightness);
		void setAutoSet(bool bAuto);
		void setAutoOn(bool bAuto);
		void setModePPS(bool bPPS);
		void setCVCC(regulator_mode_et mode);
		void setCalU(uint16_t Cal, uint16_t Ref);
		void setCalI(uint16_t Cal, uint16_t Ref);
		void setVoltageStep(uint16_t VoltageStep);
		void setCurrentStep(uint16_t CurrentStep);

		uint8_t getBrightness(void);
		bool getAutoSet(void);
		bool getAutoOn(void);
		bool getModePPS(void);
		regulator_mode_et getCVCC(void);
		void getCalU(uint16_t *pCal, uint16_t *pRef);
		void getCalI(uint16_t *pCal, uint16_t *pRef);
		uint16_t getVoltageStep(void);
		uint16_t getCurrentStep(void);
	
	private:

		const uint16_t flagVal		=0x1602;
		const uint16_t flagAddress	=0x0000;
		const uint16_t DataOffset	=sizeof(uint16_t);
		const eeprom::eeprom_density_t EepromDesitiy=eeprom::eeprom_32kbit;
		const uint8_t EepromAddress	=0x50;
	
		static const uint8_t PROGMEM pgmFF[];
	
		typedef struct parameter_s 
						{ 	uint16_t 	CalU;		// Calibration for Output Voltage
							uint16_t	RefU;		// internal measurement for Calibration
							uint16_t 	CalI;		// Calibration for output current
							uint16_t	RefI;		// internal measurement for calibration
							uint16_t	setU;		// start value voltage *20mV
							uint16_t	setI;		// start value current *50mA
							uint8_t		Brightness; // Brighness of display
							uint8_t		PPS		:1;	// slect fix or pps profile
							uint8_t		autoSet	:1;	// Flag for automatic setup to last settings on startup
							uint8_t		autoOn	:1;	// implies autoSet but switches output on startup
							uint8_t		CVCC	:2;	// Flag for active Constant Voltage / Current Regulator
													// needs support for PPS
							uint8_t		dir		:1;	// indicator for rotation memory usage
						} parameter_st;
	
						
						
		typedef enum:uint8_t {	para_start, 
								para_init,
								para_clear,
								para_write_flag,
								para_write_first,
								para_write, 
								para_read_start,
								para_read,
								para_idle,
								} para_stat_et;



		eeprom Eeprom{eeprom::eeprom_256kbit,0x50};
		
		
		
		parameter_st Parameter;
		uint16_t ParameterPos;
		uint16_t Address;
		uint16_t lowPos;
		uint16_t midPos;
		uint16_t highPos;
		para_stat_et paramState;
		bool 	 bDirFlag;
		
		uint16_t calcParamCnt(void);
		uint16_t calcParamAddress(uint16_t ParamPos);
		void nextPos(bool bUp);
		void initPos(void);
		bool getDirection(void);
		uint16_t calcPercentage(uint16_t part, uint16_t total);
		
		void setInitData(void);
};

#endif //_paramter_h_
