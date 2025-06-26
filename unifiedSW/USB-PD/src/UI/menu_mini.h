#ifndef __menu_mini_c_h__
#define __menu_mini_c_h__

#include <stdint.h>
#include "../display/ST7735_mini/tft_mini.h"
#include "../button/push_button.h"
#include "../tool/IIR.h"
#include "../../controller.h"
#include "../../parameter.h"
#include "../../Version.h"

class menu_mini_c
{
	public:
		menu_mini_c(controller_c *  controller, parameter * Parameter);
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
	
								
		typedef enum:uint8_t {	menu_item_power,
								menu_item_profile,
								menu_item_settings,
								menu_item_calibration,
								menu_item_delete,
								menu_item_remote,
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

		static const uint8_t ICON_LENGTH 	=8;		// 8 byte per icon
		static const uint8_t ICON_OFFSET 	=2;		// icon 0 & 1 used for progress bar
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

		static const char pgmTextStart[];	
		static const char pgmTextPower[];	
		static const char pgmTextProfile[];
		static const char pgmTextSettings[];        
		static const char pgmTextCalibration[];        
		static const char pgmTextRemote[];        
		static const tft_mini_c::icon_et pgmIconsPower[];
		static const tft_mini_c::icon_et pgmIconsProfile[]; 
		static const tft_mini_c::icon_et pgmIconsSettings[]; 
		static const tft_mini_c::icon_et pgmIconsCalibration[]; 
		static const menu_pos_st pgmMenuPower[];
		static const menu_pos_st pgmMenuProfile[];
		static const menu_pos_st pgmMenuSettings[];
		static const menu_pos_st pgmMenuCalibration[];
		static const menu_pos_st pgmMenuDelete[];
				
		controller_c * controller;
		parameter *	Parameter;
		
		tft_mini_c		Lcd{LCD_CAT4004_PIN,LCD_CAT4004_TYP};

		push_button_c button_ok{HWCFG_SWITCH_OK_SELECT,true,false};
		push_button_c button_up{HWCFG_SWITCH_UP,true,false};
		push_button_c button_dn{HWCFG_SWITCH_DOWN,true,false};

		
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
		bool			force_update;		// first call of menu

		
		power_edit_st		power_edit;
		profile_edit_st		profile_edit;
		settings_edit_st	settings_edit;
		calibration_edit_st	calibration_edit;

		Version_c			Version;

		char				lineBuffer[14];
		
		void draw_menu(const char * menuText, const tft_mini_c::icon_et * iconList);
		void process_button(const menu_pos_st * menuPos);
		void next_menu_item(const menu_pos_st * menuPos);
		void set_cursor(uint8_t x, uint8_t y);
		uint8_t menu_pos_items(const menu_pos_st * menuPos);
		
		void doPower(void);
		void doProfile(void);
		void doSettings(void);
		void doCalibration(void);
		void doDelete(void);
		void doRemote(void);
		
		void setOptions(void);		
};

#endif //__menu_mini_c_h__
