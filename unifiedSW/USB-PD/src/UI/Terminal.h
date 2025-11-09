#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "../VT100/VT100.h"
#include "../modules/controller.h"
#include "../modules/parameter.h"
#include "../modules/log.h"
#include "../modules/program.h"

class Terminal_c
{
	public:
		void init(	VT100_c * term, 
					controller_c * Controller, 
					parameter * Parameter, 
					log_c * Log,
					program_c * Program);

		void begin(void);
		bool process(void);
		void end(void);
	private:

		typedef enum:uint8_t {	START, PARAMETER, CONTROLLER, POWER, 
								WAITING, EDIT, LIST, SCROLL, DIR} menuMode_et;
		typedef enum:uint8_t {	IDLE, CURRENT, VOLTAGE, CALIBRATION,
								STARTUP, PROFILE, REGULATOR, OUTPUT, 
								ERASE, LOG, PROG} menuItem_et;
								

        typedef struct listMenu_s { uint8_t Xpos;
                                    uint8_t Ypos;
                                    uint8_t Size;
                                    uint8_t Items;
                                    uint8_t Last;
                                    uint8_t Selection;
                                    uint8_t Highlighted;
                                    const char *  Data;
                                    dir_c	*pDir;
                                    bool    Horizontal;
                                    bool    updated;
                                    } listMenu_st;

		static const uint8_t VoltageDisplayXpos		=2;
		static const uint8_t VoltageDisplayYpos		=1;
		static const uint8_t VoltageDisplayXsize	=10;
		static const uint8_t VoltageDisplayYsize	=3;
		static const uint8_t VoltageDisplayDouble	=true;
		static const uint8_t VoltageDisplaySize		=6;
		static const uint8_t VoltageDisplayDecimals	=3;
		static const char VoltageDisplayUnit[];

		static const uint8_t CurrentDisplayXpos=VoltageDisplayXpos+VoltageDisplayXsize-1;
		static const uint8_t CurrentDisplayYpos=1;
		static const uint8_t CurrentDisplayXsize=10;
		static const uint8_t CurrentDisplayYsize=3;
		static const uint8_t CurrentDisplayDouble=true;
		static const uint8_t CurrentDisplaySize		=6;
		static const uint8_t CurrentDisplayDecimals	=3;
		static const char CurrentDisplayUnit[];

		static const uint8_t UBusDisplayXpos= CurrentDisplayXpos+CurrentDisplayXsize-1;
		static const uint8_t UBusDisplayYpos=1;
		static const uint8_t UBusDisplayXsize=10;
		static const uint8_t UBusDisplayYsize=3;
		static const uint8_t UBusDisplayDouble=true;
		static const uint8_t UBusDisplaySize		=6;
		static const uint8_t UBusDisplayDecimals	=3;

		static const uint8_t TemperatureDisplayXpos=UBusDisplayXpos+UBusDisplayXsize-1;
		static const uint8_t TemperatureDisplayYpos= 1;
		static const uint8_t TemperatureDisplayXsize=10;
		static const uint8_t TemperatureDisplayYsize=3;
		static const uint8_t TemperatureDisplayDouble=true;
		static const uint8_t TemperatureDisplaySize		=5;
		static const uint8_t TemperatureDisplayDecimals	=1;

		
		static const char TemperatureDisplayUnit[];


		static const uint8_t OutputMenuXpos=		1;
		static const uint8_t OutputMenuYpos=		7;
		static const uint8_t OutputMenuSize=		13;
		static const uint8_t OutputMenuItems=		3;
		static const uint8_t OutputMenuLast=		1;
		static const char OutputMenuData[];
		static const uint8_t VoltageEditXpos=		3;
		static const uint8_t VoltageEditYpos=		10;
		static const uint8_t VoltageEditLen=		5;
		static const uint8_t CurrentEditXpos=		4;
		static const uint8_t CurrentEditYpos=		12;
		static const uint8_t CurrentEditLen=		4;
		static const uint8_t OutputDecimals=		2;
		static const uint8_t OutputStateXpos=		2;
		static const uint8_t OutputStateYpos=		14;

		static const uint8_t LoggingMenuXpos=		 1;
		static const uint8_t LoggingMenuYpos=		16;
		static const uint8_t LoggingMenuSize=		13;
		static const uint8_t LoggingMenuItems=	 	 2;
		static const uint8_t LoggingMenuLast=		 1;
		static const uint8_t LogIntervalXpos=		 3;
		static const uint8_t LogIntervalYpos=		19;
		static const uint8_t LogIntervalLen=		11;
		static const uint8_t LogFileNumXpos=		 3;
		static const uint8_t LogFileNumYpos=		21;
		static const uint8_t LogFileNumLen=			11;
				
		static const char LoggingMenuData[];
		static const char LoggingNoCard[];

		static const uint8_t RegulatorMenuXpos=	14;
		static const uint8_t RegulatorMenuYpos=	7;
		static const uint8_t RegulatorMenuSize=	13;
		static const uint8_t RegulatorMenuItems=4;
		static const uint8_t RegulatorMenuLast=	1;
		static const char RegulatorMenuData[];

		static const uint8_t ProgSelMenuXpos=	14;
		static const uint8_t ProgSelMenuYpos=	18;
		static const uint8_t ProgSelMenuSize=	13;
		static const uint8_t ProgSelMenuItems=	1;
		static const uint8_t ProgSelMenuLast=	1;
		static const char 	 ProgSelMenuData[];
		static const char 	 ProgSelNoFiles[];
		static const char 	 ProgSelNoCard[];
		static const char 	 ProgSelOff[];
		static const char 	 ProgSelClr[];
		static const char	 ProgFileNameXpos=	16;
		static const char	 ProgFileNameYpos=	21;
		static const char	 ProgFileNameLen=	11;
		
		static const uint8_t ProfileMenuXpos=	27;
		static const uint8_t ProfileMenuYpos=	7;
		static const uint8_t ProfileMenuSize=	26;
		static const uint8_t ProfileMenuLines=  14;
		static const uint8_t ProfileMenuItemsS=	0;
		static const uint8_t ProfileMenuLastS=	0;
		static const uint8_t ProfileMenuMax= 	7;	// max. 7 PD Profiles
		static const uint8_t ProfileMenuItemLen	=ProfileMenuSize-2;
		static const uint8_t ProfileMenuDataSize=	(ProfileMenuMax+1) * 	// max profiles + heading
													(ProfileMenuItemLen 	// Text width
														+1          ) 		// + term 0x00
													+ 1;						// + final trem 0x00
		static const uint8_t ProfileMenuOffsetText	=1;
		static const uint8_t ProfileMenuOffsetUmin	=0;
		static const uint8_t ProfileMenuOffsetUmax	=8;
		static const uint8_t ProfileMenuOffsetImax	=18;
		static const uint8_t ProfileMenuOffsetDash	=6;
		static const uint8_t ProfileMenuOffsetSlash	=16;
		static const uint8_t ProfileMenuOffsetV		=14;
		static const uint8_t ProfileMenuOffsetA		=23;
		static const uint8_t ProfileMenuValSizeV	=5;
		static const uint8_t ProfileMenuValSizeA	=4;
		static const uint8_t ProfileMenuValDecimals	=2;


		static const char ProfileMenuDataS[];

		static const uint8_t AutoStartMenuXpos=	53;
		static const uint8_t AutoStartMenuYpos=	 7;
		static const uint8_t AutoStartMenuSize=	28;
		static const uint8_t AutoStartMenuItems= 3;
		static const uint8_t AutoStartMenuLast= 1;
		static const char AutoStartMenuData[];

		static const uint8_t CalibrationMenuXpos=	53;
		static const uint8_t CalibrationMenuYpos=	12;
		static const uint8_t CalibrationMenuSize=	28;
		static const uint8_t CalibrationMenuItems=	 2;
		static const char 	 CalibrationMenuData[];
		static const uint8_t CalibrationIntXpos	=	59;
		static const uint8_t CalibrationIntYpos	=	15;
		static const uint8_t CalibrationEditXpos=	73;
		static const uint8_t CalibrationEditYpos=	15;
		static const uint8_t CalibrationEditLen=	5;
		static const uint8_t CalibrationDecimals=	3;

		static const uint8_t EraseDataMenuXpos	=	53;
		static const uint8_t EraseDataMenuYpos	=	17;
		static const uint8_t EraseDataMenuXSize	=	28;
		static const uint8_t EraseDataMenuYSize	=	 3;
		static const bool	 EraseDataMenuDouble=	false;
		static const char 	 EraseDataMenuData[];
		static const uint8_t EraseDataEditXpos	=	76;
		static const uint8_t EraseDataEditYpos	=	18;
		static const uint8_t EraseDataEditLen	=	1;


		static const uint8_t TitleXpos=	54;
		static const uint8_t TitleYpos=	24;
		static const char TitleData[];
		static const uint8_t VersionXpos=	74;
		static const uint8_t VersionYpos=	24;

/*
		static const uint8_t VersionXpos=	53;
		static const uint8_t VersionYpos=	7;
		static const uint8_t VersionSize=	14;
		static const uint8_t VersionItems=	1;
		static const char VersionData[];
		static const uint8_t BuiltXpos=		67;
		static const uint8_t BuiltYpos=		7;
		static const uint8_t BuiltSize=		14;
		static const uint8_t BuiltItems=	1;
		static const char BuiltData[];
*/

		static const uint8_t DebugProfileXpos=		54;
		static const uint8_t DebugTransitionXpos=	DebugProfileXpos+4;
		static const uint8_t DebugReadyXpos=		DebugTransitionXpos+11;
		static const uint8_t DebugYpos=	23;

		static const uint8_t ProgMsgXpos=1;
		static const uint8_t ProgMsgYpos=23;
		static const uint8_t ProgMsgLen=50;

		static const char WaitMenu[];
		static const char VerticalMenu[];
		static const char HorizontalMenu[];
		static const char EditMenu[];
		static const char YesNoMenu[];
		static const uint8_t StatusXpos= 1;
		static const uint8_t StatusXposEnd= 52;
		static const uint8_t StatusYpos=24;

		static const char NumEditChars[];
		static const char YesNoChars[];
		static const char MainMenuChars[];
		static const char OutputOff[];
		static const char OutputOn[];
		static const char OutputRun[];
		
        static const uint32_t HorizontalControl=    VT100_c::FLAG_CRSR_LEFT     |
                                                    VT100_c::FLAG_CRSR_RIGHT    |
                                                    VT100_c::FLAG_ESC           |
                                                    VT100_c::FLAG_ENTER;
        static const uint32_t VerticalControl=      VT100_c::FLAG_CRSR_UP       |
                                                    VT100_c::FLAG_CRSR_DOWN     |
                                                    VT100_c::FLAG_ESC           |
                                                    VT100_c::FLAG_ENTER;
        static const uint32_t EditControl=          VT100_c::FLAG_CRSR_LEFT     |
                                                    VT100_c::FLAG_CRSR_RIGHT    |
                                                    VT100_c::FLAG_CRSR_HOME     |
                                                    VT100_c::FLAG_CRSR_END      |
                                                    VT100_c::FLAG_BS            |
                                                    VT100_c::FLAG_DEL           |
                                                    VT100_c::FLAG_ESC           |
                                                    VT100_c::FLAG_ENTER;


		static const VT100_c::colour_et default_text_colour=VT100_c::white;
		static const VT100_c::colour_et default_background_colour=VT100_c::black;
		static const VT100_c::colour_et current_limiting_colour=VT100_c::red;
		static const VT100_c::colour_et regulated_colour=VT100_c::cyan;
		static const VT100_c::colour_et unregulated_colour=VT100_c::yellow;
		static const VT100_c::colour_et fixed_colour=VT100_c::green;
		static const VT100_c::colour_et prg_ok_colour=VT100_c::green;
		static const VT100_c::colour_et prg_err_colour=VT100_c::red;

		static const uint8_t 	millisDecimals	=3;
		static const uint8_t 	TEXT_LEN		=15;
		static const uint16_t	UPDATE_CYCLE	=300;

		static const uint8_t 	decisDecimals	=1;
		
		static const uint16_t	Vbus_limit_min	=10;	// 10mV delta before action
		static const uint16_t	Vbus_limit_max	=100;	// 100mV delta before immedeate action
		static const uint8_t	Vbus_cycle_max	=3;		// update cycles with deviation before action

		VT100_c 		*pTerminal;
		controller_c 	*pController;
		parameter		*pParameter;
		log_c			*pLog;
		program_c		*pProgram;

		uint16_t	lastUpdate;
		uint16_t	TimeStamp;
		bool		forceUpdate;
        bool        externalUpdate;
		bool		hasSD;
		bool		prgBusy;
		bool		prgRunning;

		uint8_t ProfileMenuItems;
		uint8_t ProfileMenuLast;
		char ProfileMenuData[ProfileMenuDataSize];


		char buffer[TEXT_LEN+1];
		VT100_c::colour_et colour=VT100_c::green;

		menuMode_et MenuMode;
		menuItem_et	MenuItem;



        uint16_t outputVoltageEdit;
        uint16_t outputCurrentEdit;
        uint16_t CalibrationCurrentInternal;
        uint16_t CalibrationCurrentEdit;
        uint16_t VbusVoltage;
        uint8_t	 VbusCycleCnt;
        bool     bOutputSwitch;
        bool     bAutoSet;
        bool     bAutoOn;
        uint8_t  ProfileSelection;
        uint8_t  RegulatorSelection;
		uint8_t	 LogIntervalSelection;
		uint16_t ProgFileNum;
		program_c::message_type_et program_message_type;
		uint8_t	 program_message_id;

        listMenu_st listMenuData;

		void ShowMenu(void);

		void ClearBlock(uint8_t Xpos, uint8_t Ypos, uint8_t width, uint8_t lines);
		void DrawBox(uint8_t Xpos, uint8_t Ypos, uint8_t Xsize, uint8_t YSize, bool doubleSize);
		void DrawVerticalFrame(	uint8_t Xpos, uint8_t Ypos,	// Position of the List Menu Frame
                                uint8_t Size, 				// X-width of the Frame
                                uint8_t Items=1, 				// numebr of entries (excluding Header)
                                uint8_t Last=1);			// hight of last entry
		void DrawHorizontalFrame(uint8_t Xpos, uint8_t Ypos, uint8_t Size, uint8_t Items);

		void DrawUpperLine(uint8_t Length);
		void DrawFieldLine(uint8_t Length);
		void DrawIntermediateLine(uint8_t Length, bool doubleLine);
		void DrawLowerLine(uint8_t Length);
		void DrawHeaderBox(void);
		void SetTextAttribute(	bool Selected,
								bool Highlighted,
								bool Alert=false);

		void DrawVerticalText(	uint8_t Xpos, uint8_t Ypos,	// Position of List Menu
                                const char * Data,			// pointer to List Menu data
                                uint8_t Selected=0,			// number of slected item 0=none
                                uint8_t Highlighted=0,		// number of higlghtes item 0=none
                                uint8_t Last=1,				// hight of last item
                                uint8_t ItemCnt=0);			// mumber of items in Data list exlcuding Header;

		void DrawVerticalText(	uint8_t Xpos, uint8_t Ypos,	// Position of List Menu
                                uint8_t Item,				// Position of Item
                                const char * Data,			// pointer to List Menu data
                                bool Selected=false,		// item is slected
                                bool Highlighted=false,		// item is highlighted
                                bool last=false);			// item is multi line

		void DrawHorizontalText(uint8_t Xpos, uint8_t Ypos,	// Position of decision Menu
								uint8_t Size, 				// Width of deceision Menu
								const char * Data, 			// pointer to decision Menu Data
								uint8_t Selected=0, 		// number of higlghtes item 0=none
								uint8_t Highlighted=0);     // mumber of items in Data list exlcuding Header;

		void DrawHorizontalText(uint8_t Xpos, uint8_t Ypos,	// Position of decision Menu
								uint8_t Size, uint8_t Items,// Width of deceision Menu, number of items
								uint8_t Item,
								const char * Data, 			// pointer to decision Menu Data
								bool Selected=false, 	// number of higlghtes item 0=none
								bool Highlighted=false);   // mumber of items in Data list exlcuding Header;

		void DrawDirText(		uint8_t Xpos, uint8_t Ypos,	// Position of decision Menu
								dir_c *pDir,
								uint8_t Selection, 			// current entry
								bool Selected=false, 		// selction active
								bool blink=false,			// attribut blink
								VT100_c::colour_et colour = 
									default_text_colour);	// display colour
								

		void DrawBoxText(	uint8_t Xpos, uint8_t Ypos,
							const char* Data,
							bool doubleSize,
							uint8_t YSize=3);

		void DrawStatusLineText(const char *Data);

		void DrawScrollText(	uint8_t Xpos, uint8_t Ypos,	// of text field
								const char * Data, 			// pointer to decision Menu Data
								uint8_t Selected,	 		// number of item to display
								bool Highlighted=false);    // display inverted or not;


		uint8_t CountMenuData(const char * Data);
		const char * NextMenuItem(const char * Data);

		void printTitle(void);
		void printVersion(void);

		void updateData(void);
		void updateOutputReadings(uint16_t Voltage_mV, int16_t Current_mA, bool on, bool pps, bool cc, bool regulated);
		void updateSetVoltage(uint16_t Voltage_mV);
		void updateSetCurrent(uint16_t Current_mA);
		void updateInVoltage(uint16_t Voltage_mV);
		void updateOutputSwitch(bool bOn);
		void updateCalibration(uint16_t Internal_mA, uint16_t Reference_mA);
		void updateAutoStart(bool bAutoSet, bool bAutoOn);
		void updateRegulator(controller_c::operating_mode_et mode);
		void updatePower(uint8_t profile);
		void updateTemperature(int16_t temp_dC);
		void updateLogging(log_c::logInterval_et interval, uint32_t logFileNum, bool hasSD);
		void updateProgSel(uint16_t FileNum);
		void updateProgMsg(void);

		void createPower(void);
		void resetPower(void);

        void StartListMenu(uint8_t Xpos, uint8_t Ypos, const char * Data, uint8_t Size, uint8_t Last, uint8_t Selection, bool Horizontal=false);
        bool ProcessListMenu(void);
        void EndListMenu(menuMode_et &mode,	menuItem_et &item);
		void StartEditNum(uint8_t Xpos, uint8_t Ypos, uint16_t val, uint8_t Size, uint8_t Decimals);
		void StartEditYesNo(uint8_t Xpos, uint8_t Ypos, char val);
        bool ProcessEdit(void);
		void EndEdit(menuMode_et &mode,	menuItem_et &item);
		void StartScrollMenu(uint8_t Xpos, uint8_t Ypos, const char * Data, uint8_t Size, uint8_t Selection);
		bool ProcessScrollMenu(void);
		void EndScrollMenu(menuMode_et &mode,menuItem_et &item);
		void StartDirMenu(uint8_t Xpos, uint8_t Ypos, dir_c * dir, uint8_t Size, uint8_t Selection);
		bool ProcessDirMenu(void);
		void EndDirMenu(menuMode_et &mode,menuItem_et &item);

        void processWaiting(menuMode_et &mode,	menuItem_et &item);
		void StartMenu(menuItem_et &item);

		void SetOutputSwitch(void);
		uint16_t SetOutputVoltage(uint16_t Voltage_mV);
		uint16_t SetOutputCurrent(uint16_t Current_mA);
		void SetCalibration(uint16_t Ref_mA);
        menuMode_et SetProfile(uint8_t Profile);
        void SetRegulator(uint8_t Mode);
        void SetAutoStart(uint8_t Mode);
		void SetLogging(uint8_t Mode);
		void SetProgram(uint8_t FileNum);
		
        uint8_t getAutoStartupVal(bool AutoSet, bool AutoOn);
        void setAutoStartup(uint8_t val, bool &AutoSet, bool &AutoOn);

};
#endif //__TERMINAL_H__
