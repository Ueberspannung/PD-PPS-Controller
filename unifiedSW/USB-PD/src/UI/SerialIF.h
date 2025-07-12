#ifndef __SERIALIF_H__
#define __SERIALIF_H__

#include "../tool/SerialClass.h"
#include "../../controller.h"
#include "../../parameter.h"

class SerialIF_c
{
	public:
		void init(SerialClass * com, controller_c * Controller, parameter * Parameter);

		void begin(void);
		bool process(void);
		void end(void);
	private:

		typedef enum:uint8_t {START, PARAMETER, CONTROLLER, POWER, WAITING, ERASE, CHANGE} processMode_et;

		static const char ReadCommands[];
		static const char WriteCommands[];

		static const char StatusTextReady[];
		static const char StatusTextBusy[];
		static const char StatusTextChange[];
		static const char StatusTextErase[];
		static const char StatusTextPower[];
		static const char StatusTextParameter[];

		static const char RegulatorModes[];
		static const char AutoStartModes[];
		static const char PowerModes[];

		static const char ReadFlag='?';
		static const char WriteFlag='!';

		static const uint8_t BufferLen 	= 48;
		static const uint8_t RxMin		= 2;

		static const uint8_t millisDecimals	=3;
		static const uint8_t millisLen		=6;
		static const uint8_t ProfileDecimals=2;
		static const uint8_t ProfileVoltage =5;
		static const uint8_t ProfileCurrent =4;

		static const uint16_t minSendIntervall=100;	

		char Buffer[BufferLen];
		uint8_t BufferCnt;
		uint16_t TimeStamp;
		uint16_t SendIntervall;
		uint16_t LastSend;
        bool externalUpdate;

		processMode_et processMode;

		SerialClass *	pPort;
		controller_c *	pController;
		parameter *		pParameter;

		bool bReceive(void);
		void DecodeCommand(void);
		bool bFind(const char Val, const char * Set);
		void cpy(char * dest, uint8_t & destPos, const char *src);
		void addChar(char * dest, uint8_t & destPos, const char Val, uint8_t cnt=1);

		uint8_t CountData(const char * Data);
		const char * NextItem(const char * Data);
		const char * GetItem(const char * Data,uint8_t n);
		uint8_t FindItem(const char * Data, const char * Items);


		void doSystemStatus(void);
		void doProfile(void);
		void doOutputStatus(void);
		void doCurrentCalibration(void);
		void doEraseParameter(void);
		void doFindProfile(void);
		void doCurrent(void);
		void doPowerInfo(void);
		void doOutputSwitch(void);
		void doRegulatorSettings(void);
		void doStartUpSettings(void);
		void doPowerType(void);
		void doVoltage(void);
		void doVersion(void);
};

#endif // __SERIALIF_H__
