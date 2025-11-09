#ifndef _parameter_h_
#define _parameter_h_
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "../memory/eeprom.h"
#include "controller.h"
#include "log.h"

class parameter
{
	public:

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
		void setCVCC(controller_c::operating_mode_et mode);
		void setCalU(uint16_t Cal, uint16_t Ref);
		void setCalI(uint16_t Cal, uint16_t Ref);
		void setVoltage_mV(uint16_t VoltageStep);
		void setCurrent_mA(uint16_t CurrentStep);
		void setLogInteval(log_c::logInterval_et interval);
		void setExtendedMenu(bool bExtended);
		void setProgramName(const char * pName);
		void setFlags(const uint8_t *pFlags);

		uint8_t getBrightness(void);
		bool getAutoSet(void);
		bool getAutoOn(void);
		bool getModePPS(void);
		controller_c::operating_mode_et getCVCC(void);
		void getCalU(uint16_t *pCal, uint16_t *pRef);
		void getCalI(uint16_t *pCal, uint16_t *pRef);
		uint16_t getVoltage_mV(void);
		uint16_t getCurrent_mA(void);
		log_c::logInterval_et getLogInterval(void);
		bool getExtendedMenu(void);
		void getProgramName(char * pName);
		const char * getProgramName(void);
		void getFlags(uint8_t *pFlags);

		bool hasSD(void);

        bool isBusy(void) {return (paramState!=para_idle);}
	private:

		const uint16_t flagVal		=0xABBA;	// used flags: 0x1602, 0xAFFE
		const uint16_t flagAddress	=0x0000;
		const uint16_t DataOffset	=sizeof(uint16_t);
		const eeprom::eeprom_density_t EepromDesity=HWCFG_EEPROM_DENSITY;
		const uint8_t EepromAddress	=HWCFG_EEPROM_I2C_ADDR;

		static const uint8_t PROGMEM pgmFF[];

		static const char 		configFileName[];
		static const char 		configIDList[];
		static const uint8_t	configBufferSize=64;
		
		static const uint8_t	ProgramNameLenght=8;
		static const uint8_t	FlagNumber=10;
		
		typedef struct parameter_s
						{ 	uint16_t 	CalU;		// Calibration for Output Voltage
							uint16_t	RefU;		// internal measurement for Calibration
							uint16_t 	CalI;		// Calibration for output current
							uint16_t	RefI;		// internal measurement for calibration
							uint16_t	setU;		// start value voltage
							uint16_t	setI;		// start value current
							uint8_t		Brightness; // Brighness of display
							char		ProgramName[ProgramNameLenght+1];
							uint8_t		Flags[FlagNumber];
							uint8_t		PPS		:1;	// slect fix or pps profile
							uint8_t		autoSet	:1;	// Flag for automatic setup to last settings on startup
							uint8_t		autoOn	:1;	// implies autoSet but switches output on startup
							uint8_t		CVCC	:2;	// Flag for active Constant Voltage / Current Regulator
													// needs support for PPS
							uint8_t		Menu	:1;	// selects standard / extended menu
							uint8_t		Logging :4;	// Logging Mode 0-9 (off-30s)
							uint8_t		dir		:1;	// indicator for rotation memory usage
						} parameter_st;



		typedef enum:uint8_t {	para_start,
								para_init,
								para_clear,
								para_write_flag,
								para_write_first,
								para_write,
								para_read,
								para_idle,
								} para_stat_et;


		typedef enum:uint8_t { 	ITEM_CAL_U = 0,
								ITEM_REF_U,
								ITEM_CAL_I,
								ITEM_REF_I,
								ITEM_SET_U,
								ITEM_SET_I,
								ITEM_BRIGHTNESS,
								ITEM_PROGRAM_NAME,
								ITEM_FLAGS,
								ITEM_PPS,
								ITEM_AUTO_SET,
								ITEM_AUTO_ON,
								ITEM_REGULATOR,
								ITEM_LOGGING,
								ITEM_MENU,
								ITEM_END } parameter_et;

		typedef enum:uint8_t {	NO_MEMORY,
								EEPROM,
								SDCARD } memory_et;
								
        typedef enum:uint8_t {  SD_START,
                                SD_DELETE,
                                SD_OPEN,
                                SD_DATA,
                                SD_CLOSE,
                                SD_IDLE } sdcard_stat_et;

		eeprom Eeprom{EepromDesity,EepromAddress};

		File		configFile;
		char		configBuffer[configBufferSize];
		parameter_et parameterIterator;
		sdcard_stat_et sdState;

		parameter_st Parameter;
		uint16_t ParameterPos;
		uint16_t Address;
		uint16_t lowPos;
		uint16_t midPos;
		uint16_t highPos;
		para_stat_et paramState;
		bool 	 bDirFlag;
		memory_et memoryType;



		uint16_t calcParamCnt(void);
		uint16_t calcParamAddress(uint16_t ParamPos);
		void nextPos(bool bUp);
		void initPos(void);
		bool getDirection(void);
		uint16_t calcPercentage(uint16_t part, uint16_t total);

		void setInitData(void);

		void process_para_start(void);
		void process_para_init(void);
		void process_para_clear(void);
		void process_para_write_flag(void);
		void process_para_write_first(void);
		void process_para_write(void);
		void process_para_read(void);

		const char* getConfigID(parameter_et param);
		parameter_et findConfigID(const char * configLine);
		const char* getNextConfigID(const char *List);
		uint8_t getConfigIDLen(parameter_et param);

        void processReadConfig(void);
		void readConfigLine(void);
        void processWriteConfig(void);
		void writeConfigLine(void);
		
};

#endif //_paramter_h_
