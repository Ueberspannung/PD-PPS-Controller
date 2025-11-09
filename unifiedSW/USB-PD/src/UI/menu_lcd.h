#ifndef __menu_lcd__
#define __menu_lcd__

#include <stdint.h>
#include "../display/HD44780/lcd.h"
#include "../button/rotary_button.h"
#include "../tool/IIR.h"
#include "../tool/Version.h"
#include "../modules/controller.h"
#include "../modules/parameter.h"
#include "../modules/log.h"

class menu_lcd_c
{
	public:
		menu_lcd_c(controller_c *  controller, parameter * Parameter, log_c * Log);
		void process(void);
		
		void setRemote(bool bRemote);
		void forceUpdate(void);
		
	private:
	
		typedef enum:uint8_t {	menu_init_start,
								menu_init_delay,
								menu_read_parameter,
								menu_get_pd_data,
								menu_set_parameter,
								menu_start,
								menu_process,
								menu_wait_pd_transition,
								menu_end
								}	menue_state_et;
					
		typedef enum:uint8_t {	icon_switch,
								icon_ramp,
								icon_flash,
								icon_settings,
								icon_calibration,
								icon_contrast,
								icon_regulator,
								icon_ok,
								icon_log,
								icon_delete,
								icon_sdcard,
								icon_sdcard_active,
								icon_prg,
								icon_step,
								icon_file,
								icon_menu,
								icon_right,
								icon_left,
								icon_down,
								icon_up,
								icon_degree,
								icon_cancel,
								icon_end 
								}	menu_icon_et;
								
		typedef enum:uint8_t {	menu_item_power,
								menu_item_profile,
								menu_item_settings,
								menu_item_calibration,
								menu_item_delete,
								menu_item_remote,
								menu_item_power_standard,
								menu_item_file,
								menu_item_menu_end,
								menu_item_switch,
								menu_item_ok,
								menu_item_cancel,
								menu_item_activate1,
								menu_item_activate2,
								menu_item_activate3,
								menu_item_edit,
								menu_item_radio1,
								menu_item_radio2,
								menu_item_radio3,
								menu_item_radio4,
								menu_item_radio5,
								menu_item_radio6,
								menu_item_end
								} menu_item_et;
								
		typedef enum:uint8_t {	menu_mode_off,				// cursor is off
								menu_mode_select,			// cursor is on, slecet menue item
								menu_mode_select_edit,		// change cursor to blink, lock movement to field
								menu_mode_select_edit_pos,	// position of editing, select when blinking
								menu_mode_edit				// on editing position toggle between select and edit
								} menu_mode_et;

		typedef struct menu_pos_s {	menu_item_et	menu_item;
									menu_item_et	lock_item;
									menu_mode_et	menu_action;
									uint8_t			x_pos;
									uint8_t			y_pos;
									uint16_t		edit_val;
									} menu_pos_st;
					
		
		typedef struct button_s { 	uint8_t	buttonDown:1;
									uint8_t	buttonPressed:1;
									uint8_t	buttonHeld:1;
									int8_t	buttonTurns;
								} button_st;

		typedef struct power_limits_s	{	uint16_t	max_v;
											uint16_t	min_v;
											uint16_t	max_i;
										}	power_limits_st;
										
		typedef struct power_edit_s		{	uint16_t	uSet;
											uint16_t	iSet;
											uint16_t	uEdit;
											uint16_t	iEdit;
											uint16_t	uOut;
											int16_t		iOut;
											IIR<int16_t,int32_t> iOutAvg{3};
											IIR<uint16_t,uint32_t> uOutAvg{3};
											bool		output;
											} power_edit_st;

		typedef struct profile_edit_s	{	uint8_t		maxProfile;
											uint8_t		currentProfile;
											uint8_t		lastProfile;
											PD_power_info_t	PowerInfo;
											power_limits_st PowerLimits;
											} profile_edit_st;

		typedef struct settings_edit_s	{	uint8_t		AutoSet:1;
											uint8_t		CVCC:2;
											uint8_t		AutoOn:1;
											uint8_t		MenuExt:1;
											uint8_t		Brightness;
											} settings_edit_st;

		typedef struct callibration_value_s	{	uint16_t	internal_value;
												uint16_t	reference_value;
											}	calibration_value_st;
		typedef struct calibration_edit_s {	calibration_value_st 	setCurrent;
											calibration_value_st 	editCurrent;
											bool					measure;
											IIR<uint16_t,uint32_t>	Filter{4};
											uint16_t				Timer;
											} calibration_edit_st;
		typedef struct file_edit_s {	log_c::logInterval_et 	logInterval;
										uint32_t				logFileNum;
									}	file_edit_st;

		static const uint8_t ICON_LENGTH 	=8;		// 8 byte per icon
		static const uint8_t ICON_OFFSET 	=0;		// 2;		// icon 0 & 1 used for progress bar
		static const uint8_t ICON_CNT		=8;		// max. 8 icons
		static const uint16_t MENU_IDLE_TIME=10000;	// cursor off after 10s
		static const uint16_t UPDATE_CYCLE  =250;	// update display during operation
		static const uint16_t UPDATE_CYCLE_SET=1500;	// update display during setup
		static const uint16_t CAL_CYCLE		=50;	// update intervall for cal filter 
		static const uint16_t CAL_LIMIT_U	=30000;	// absolute maximum 30V
		static const uint16_t CAL_LIMIT_I	=6000;	// absolute maximum 6A
		static const uint16_t PD_DELAY		=200;	// 200 ms delay after transition accomplisched
		static const uint16_t PD_REFRESH	=2500;	// first zero offset 5 sec after 0A
		static const uint16_t REG_DELAY		=1;		// wait ...ms before measuring
		static const uint16_t INIT_DELAY	=5000;	// display start message

		static const uint8_t LCD_BASE			=HWCFG_LCD_I2C_ADDR;
		static const uint8_t LCD_PWM			=HWCFG_LCD_I2C_ADDR;
		static const uint8_t LCD_CAT4004_PIN	=HWCFG_CAT4004_PIN;
		static const uint8_t LCD_CAT4004_TYP	=HWCFG_CAT4004_TYPE;


		static const uint8_t icon_tab[][ICON_LENGTH] PROGMEM;

		static const char pgmTextStart[] PROGMEM;	
		static const char pgmTextPower[] PROGMEM;	
		static const char pgmTextProfile[] PROGMEM;
		static const char pgmTextSettings[] PROGMEM;        
		static const char pgmTextCalibration[] PROGMEM;        
		static const char pgmTextRemote[] PROGMEM;   
		static const char pgmTextPowerStd[] PROGMEM;	
		static const char pgmTextFile[] PROGMEM;	
		     
		static const menu_icon_et pgmIconsPower[] PROGMEM;
		static const menu_icon_et pgmIconsProfile[] PROGMEM; 
		static const menu_icon_et pgmIconsSettings[] PROGMEM; 
		static const menu_icon_et pgmIconsCalibration[] PROGMEM; 
		static const menu_icon_et pgmIconsPowerStd[] PROGMEM;
		static const menu_icon_et pgmIconsFile[] PROGMEM;
		static const menu_icon_et pgmIconsRemote[] PROGMEM;
		
		static const menu_pos_st pgmMenuPower[] PROGMEM;
		static const menu_pos_st pgmMenuProfile[] PROGMEM;
		static const menu_pos_st pgmMenuSettings[] PROGMEM;
		static const menu_pos_st pgmMenuCalibration[] PROGMEM;
		static const menu_pos_st pgmMenuDelete[] PROGMEM;
		static const menu_pos_st pgmMenuPowerStd[] PROGMEM;
		static const menu_pos_st pgmMenuFile[] PROGMEM;
				
		controller_c * 	controller;
		parameter *		Parameter;
		log_c *			Log;
		
		lcd				Lcd{LCD_BASE,LCD_PWM,LCD_CAT4004_PIN,LCD_CAT4004_TYP};
		rotary_button_c	button;
		
		menue_state_et 	main_state;
		menu_item_et	active_menu;
		menu_item_et	lock_item;
		menu_mode_et	menu_mode;
		button_st		buttonState;
		int8_t			menu_item;
		uint16_t		lastAction;		// time of last button action
		uint16_t		lastUpdate;		// time of last button action
		uint16_t		lastRefesh;		// time of last PDPPS Refresh
		uint16_t		menuTime;		// curent time of call
		bool			force_update;	// first call of menu or neww data request

		uint32_t		iconFlags;		// defined / undefined icons
		
		char 			lineBuffer[20];
		
		power_edit_st		power_edit;
		profile_edit_st		profile_edit;
		settings_edit_st	settings_edit;
		calibration_edit_st	calibration_edit;
		file_edit_st		file_edit;

		Version_c			Version;
		
		void draw_menu(const char * menuText, const menu_icon_et * iconList);
		void process_button(const menu_pos_st * menuPos);
		void next_menu_item(const menu_pos_st * menuPos);
		void set_cursor(uint8_t x, uint8_t y);
		uint8_t menu_pos_items(const menu_pos_st * menuPos);
		
		void doPower(void);
		void doPowerStandard(void);
		void doProfile(void);
		void doSettings(void);
		void doCalibration(void);
		void doFile(void);
		void doDelete(void);
		void doRemote(void);
		
		void setOptions(void);		
		
		char icon2char(const menu_icon_et * icon_list, menu_icon_et icon);
		bool redefine_icon(const menu_icon_et * icon_list, menu_icon_et icon,  menu_icon_et new_icon);

		bool is_icon_defined(menu_icon_et icon);
		void set_icon_defined(menu_icon_et icon, bool define);
};

#endif //__menu_lcd__
