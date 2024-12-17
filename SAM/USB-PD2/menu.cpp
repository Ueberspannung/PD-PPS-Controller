#include "Arduino.h"
#include "menu.h"
#include "button.h"
#include "messages.h"

#ifdef ARDUINO_ARCH_AVR
	#define DEBUG_BAUD (230400)
#else
	#define DEBUG_BAUD (115200)
#endif

#define _menu_debug_level (0)	// 0: off
								// 1: regulator steps
								// 2: configuration
#if  _menu_debug_level == 1
	#define MENU_DEBUG_INIT(x) Serial1.begin(x)
	#define MENU_DEBUG_PRINT(x) Serial1.print(x)
	#define MENU_DEBUG_PRINTLN(x) { Serial1.print(x); Serial1.print("\n"); }
	#define MENU_DEBUG_PRINT2(x)
	#define MENU_DEBUG_PRINTLN2(x)
#elif _menu_debug_level == 2
	#define MENU_DEBUG_INIT(x) Serial1.begin(x)
	#define MENU_DEBUG_PRINT(x) 
	#define MENU_DEBUG_PRINTLN(x) 
	#define MENU_DEBUG_PRINT2(x) Serial1.print(x)
	#define MENU_DEBUG_PRINTLN2(x) { Serial1.print(x); Serial1.print("\n"); }
#else
	#define MENU_DEBUG_INIT(x) 
	#define MENU_DEBUG_PRINT(x) 
	#define MENU_DEBUG_PRINTLN(x) 
	#define MENU_DEBUG_PRINT2(x)
	#define MENU_DEBUG_PRINTLN2(x)
#endif


const uint8_t menu::icon_tab[][ICON_LENGTH] PROGMEM= {   // 
			{0x04,0x04,0x0E,0x15,0x15,0x11,0x0E,0x00}, 	// (0) icon_switch
			{0x00,0x01,0x03,0x07,0x0F,0x1F,0x00,0x00}, 	// (1) icon_ramp
			{0x01,0x02,0x04,0x0F,0x1E,0x04,0x08,0x10}, 	// (2) icon_flash
			{0x11,0x11,0x1B,0x0E,0x0E,0x0E,0x0E,0x0E}, 	// (3) icon_settings
			{0x04,0x0E,0x11,0x15,0x11,0x0E,0x04,0x00},	// (4) icon_calibration
			{0x00,0x0E,0x017,0x13,0x17,0xE,0x00,0x00},	// (5) icon_contrast
			{0x00,0x11,0x12,0x1C,0x10,0x1F,0x00,0x00},	// (6) icon_regulator
			{0x00,0x00,0x01,0x02,0x14,0x08,0x00,0x00}, 	// (7) icon_ok
			{0x04,0x04,0x15,0x0E,0x04,0x11,0x11,0x1F},	// (8) icon_save
			{0x0E,0x1F,0x00,0x1F,0x15,0x15,0x15,0x0E},	// (9) icon_delete
			};
														// icon_cancel "x"
			/* *********************************************
				currently not used
			{0x00,0x00,0x00,0x00,0x00,0x15,0x00,0x00}, // ...
			{0x00,0x04,0x02,0x1F,0x02,0x04,0x00,0x00}, // ->
			{0x00,0x04,0x08,0x1F,0x08,0x04,0x00,0x00}, // <- 
			{0x00,0x04,0x04,0x15,0x0E,0x04,0x00,0x00}, // dn
			{0x00,0x04,0x0E,0x15,0x04,0x04,0x00,0x00}, // up
			*/
													//	 01234567890123456789
const char menu::pgmTextStart[] PROGMEM=				" PD/PPS-Controller  "
														"===================="
														"Ver. :              "
														"Build:              ";
			
													//   [.  .  .  .  .]  (.)
													//	 01234567890123456789
const char menu::pgmTextPower[] PROGMEM=				"Mode (x:...) [UI^] !" 
														"     UU.UU V  I.II A"
														"OUT  UU.UU V  I.II A"
														"[\x03  \x04  \x05  \x06  x]  (\x02)";

													/* ******************************
													 * alternate menu: 
													 * FIX PDO:	(LED green)
														"FIX  #n             " 
														"     UU.UU V  I.II A"
														"OUT  UU.UU V  I.II A"
														"[\x03  \x04  \x05  \x06  x]  (\x02)";

													 * ************************************** 
													 * APDO without regulation (LED yellow)
														"UU.U - UU.U V I.II A" 
														"     UU.UU V  I.II A"
														"OUT  UU.UU V  I.II A"
														"[\x03  \x04  \x05  \x06  x]  (\x02)";

													 * ************************************** 
													 * APDO wit regulation (LED BLUE)
													 * 1st line: range 
													 * 	voltage range (all APDOs combined)
													 * 	max. current at set voltage
													 * 2nd line: regulation: 
													 * 	U   :	constant voltage
													 *  UI  :	constant votage / current and 
													 * 			overcurrent protection by PDO
													 *			PDO source
													 * 	UI !:	current limiting acitve (LED red)
													 *  UI^ :	PDO source set to max. available
													 * 			current
													 *  UI^!:	current limiting active
														"UU.U - UU.U V I.II A" 
														"UI^! UU.UU V  I.II A"
														"OUT  UU.UU V  I.II A"
														"[\x03  \x04  \x05  \x06  x]  (\x02)";
													 * 
													 * 
													 * */

													//   [.  .  .]        (.)
													//	 01234567890123456789
const char menu::pgmTextProfile[] PROGMEM=				" # 1 / n  (...)     "
														"U= UU.UU V - UU.UU V"
														"I=  I.II A max      "
														"[\x03  \x04  x]        (\x02)";
														
													//   (.)=auto  (.)= 100%
													//   [. setOutput .  .  .]     (.)
													//	 01234567890123456789
const char menu::pgmTextSettings[] PROGMEM=				"(\x03)=auto  (*)=\xA5    *"
														"(\x04)=auto            "
														"(\x05)=auto            "
														"[\x03  \x06  \x07  x]     (\x02)";

													//   [.  .  .  .]     (.)
													//	 01234567890123456789
const char menu::pgmTextCalibration[] PROGMEM=			"Current Calibration "
														" internal   I.III A "
														" reference  I.III A "
														"[\x03  \x04  \x05  x]     (\x02)";




 const menu::menu_icon_et menu::pgmIconsPower[] PROGMEM = {	icon_ramp, 
															icon_switch, 
															icon_flash, 
															icon_settings,
															icon_ok, 
															icon_regulator, 
															icon_cancel};
															
 const menu::menu_icon_et menu::pgmIconsProfile[] PROGMEM = 
														{	icon_flash, 
															icon_ramp,
															icon_ok, 
															icon_cancel};

 const menu::menu_icon_et menu::pgmIconsSettings[] PROGMEM = 
														{ 	icon_settings, 
															icon_ramp, 
															icon_switch,
															icon_regulator,
															icon_calibration, 
															icon_ok, 
															icon_cancel};
															
 const menu::menu_icon_et menu::pgmIconsCalibration[] PROGMEM = 
														{ 	icon_settings, 
															icon_ramp, 
															icon_delete,
															icon_ok, 
															icon_cancel};

 const menu::menu_pos_st menu::pgmMenuPower[] PROGMEM =						
	{	{	menu_item_switch,	menu_item_switch, 	menu_mode_select,  		   1, 3,    0},
		{	menu_item_profile,	menu_item_profile,	menu_mode_select,  		   4, 3,    0},
		{	menu_item_settings,	menu_item_settings,	menu_mode_select,  		   7, 3,    0},
		{	menu_item_activate1,menu_item_activate1,menu_mode_select_edit_pos,11, 1,    0},
		{	menu_item_activate2,menu_item_activate2,menu_mode_select_edit_pos,19, 1,    0},
		{	menu_item_edit,		menu_item_activate1,menu_mode_edit,			   6, 1, 1000},
		{	menu_item_edit,		menu_item_activate1,menu_mode_edit,			   9, 1,   20},
		{	menu_item_edit,		menu_item_activate2,menu_mode_edit,			  14, 1, 1000},
		{	menu_item_edit,		menu_item_activate2,menu_mode_edit,			  16, 1,  100},
		{	menu_item_edit,		menu_item_activate2,menu_mode_edit,			  17, 1,   10},
		{	menu_item_ok,		menu_item_end, 		menu_mode_select_edit,	  10, 3,    0},
		{	menu_item_cancel,   menu_item_end,		menu_mode_select_edit,	  13, 3,    0},
		{	menu_item_end,   	menu_item_end,		menu_mode_off,			   0, 0,    0}};

const menu::menu_pos_st menu::pgmMenuProfile[] PROGMEM =						
	{	{	menu_item_power,	menu_item_power, 	menu_mode_select,	   	   1, 3, 0},
		{	menu_item_radio1,	menu_item_radio1,	menu_mode_select_edit_pos, 1, 0, 0},
		{	menu_item_ok,		menu_item_end, 		menu_mode_select_edit,     4, 3, 0},
		{	menu_item_cancel,	menu_item_end,		menu_mode_select_edit,	   7, 3, 0},
		{	menu_item_end,  	menu_item_end,		menu_mode_off,			   0, 0, 0}};
												
const menu::menu_pos_st menu::pgmMenuSettings[] PROGMEM =						
	{	{	menu_item_power,		menu_item_power, 		menu_mode_select,	   	   1, 3, 0},
		{	menu_item_calibration,	menu_item_calibration,	menu_mode_select,		   4, 3, 0},
		{	menu_item_radio1,		menu_item_radio1,		menu_mode_select_edit_pos, 1, 0, 0},
		{	menu_item_radio2,		menu_item_radio2,		menu_mode_select_edit_pos, 1, 1, 0},
		{	menu_item_radio3,		menu_item_radio3,		menu_mode_select_edit_pos, 1, 2, 0},
		{	menu_item_radio4,		menu_item_radio4,		menu_mode_select_edit_pos,11, 0, 0},
		{	menu_item_ok,			menu_item_end, 			menu_mode_select_edit,	   7, 3, 0},
		{	menu_item_cancel,		menu_item_end,			menu_mode_select_edit,	  10, 3, 0},
		{	menu_item_end,  		menu_item_end,			menu_mode_off,		   	   0, 0, 0}};

const menu::menu_pos_st menu::pgmMenuCalibration[] PROGMEM =						
	{	{	menu_item_power,		menu_item_power, 	menu_mode_select,		   1, 3,     0},
		{	menu_item_delete,		menu_item_delete,	menu_mode_select,	   	   4, 3,     0},
		{	menu_item_activate1,	menu_item_activate1,menu_mode_select_edit_pos,18, 2,     0},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  12, 2,  1000},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  14, 2,   100},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  15, 2,    10},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  16, 2,     1},
		{	menu_item_ok,			menu_item_end, 		menu_mode_select_edit,	   7, 3,     0},
		{	menu_item_cancel,		menu_item_end,		menu_mode_select_edit,	  10, 3,     0},
		{	menu_item_end,  		menu_item_end,		menu_mode_off,			   0, 0,     0}};

const menu::menu_pos_st menu::pgmMenuDelete[] PROGMEM =						
	{	{	menu_item_ok,			menu_item_end, 		menu_mode_select_edit,	   7, 3,     0},
		{	menu_item_cancel,		menu_item_end,		menu_mode_select_edit,	  10, 3,     0},
		{	menu_item_end,  		menu_item_end,		menu_mode_off,	   	   0, 0,     0}};


menu::menu(controller_c *  controller, parameter * Parameter)
{	// menu
	main_state=menu_init_start;
	this->controller=controller;
	this->Parameter=Parameter;
}	// menu

void menu::process(void)
{	// process
	static uint16_t temp;
	menuTime=controller->clock_ms();
	
	switch (main_state)
	{	// switch (main_state)
		case menu_init_start:
			MENU_DEBUG_INIT(DEBUG_BAUD);
			MENU_DEBUG_PRINTLN(F("MENU_DEBUG_LEVEL 1"));
			MENU_DEBUG_PRINTLN2(F("MENU_DEBUG_LEVEL 2"));
			 
			
			active_menu=menu_item_power;


			MENU_DEBUG_PRINTLN2(F("Button"));
			
			button_init();

			MENU_DEBUG_PRINTLN2(F("LCD"));

			Lcd.init(20,4);
			Lcd.BacklightOn(true);
			Lcd.DisplayOn(true);
			Lcd.ProgressBarInit(0,3,20,0);
			Lcd.putChar_P(0,0,pgmTextStart);
			Lcd.putChar_P(8,2,Version.pgmGetVersion());
			Lcd.putChar_P(8,3,Version.pgmGetBuiltDate());

			main_state=menu_init_delay;
			temp=menuTime;
			break;
		case menu_init_delay:
			if ((uint16_t)(menuTime-temp)>INIT_DELAY)
			{
				MENU_DEBUG_PRINTLN2("Reading Config");
				Lcd.putChar(0,2,F("reading config  ..."));
				main_state=menu_read_parameter;
			}
			break;
		case menu_read_parameter:
			if (Parameter->process())
			{	// parameter busy
				MENU_DEBUG_PRINT2(Parameter->getProgress());
				MENU_DEBUG_PRINTLN2("%");
				Lcd.ProgressSet(Parameter->getProgress());
			}	// parameter busy
			else
			{	// parameter done
				MENU_DEBUG_PRINT2("Bright ="); 	MENU_DEBUG_PRINTLN2(Parameter->getBrightness());
				MENU_DEBUG_PRINT2("AutoSet="); 	MENU_DEBUG_PRINTLN2(Parameter->getAutoSet());
				MENU_DEBUG_PRINT2("AutoOn ="); 	MENU_DEBUG_PRINTLN2(Parameter->getAutoOn());
				MENU_DEBUG_PRINT2("PPS    ="); 	MENU_DEBUG_PRINTLN2(Parameter->getModePPS());
				MENU_DEBUG_PRINT2("RegMode="); 	MENU_DEBUG_PRINTLN2(Parameter->getCVCC());
				MENU_DEBUG_PRINT2("StartU ="); 	MENU_DEBUG_PRINTLN2(Parameter->getVoltage_mV());
				MENU_DEBUG_PRINT2("StartI ="); 	MENU_DEBUG_PRINTLN2(Parameter->getCurrent_mA());
				uint16_t Cal,Ref;
				Parameter->getCalU(&Cal,&Ref);
				MENU_DEBUG_PRINT2("Cal U C="); 	MENU_DEBUG_PRINTLN2(Cal);
				MENU_DEBUG_PRINT2("Cal U R="); 	MENU_DEBUG_PRINTLN2(Ref);
				Parameter->getCalI(&Cal,&Ref);
				MENU_DEBUG_PRINT2("Cal I C="); 	MENU_DEBUG_PRINTLN2(Cal);
				MENU_DEBUG_PRINT2("Cal I R="); 	MENU_DEBUG_PRINTLN2(Ref);

				Lcd.ProgressSet(0);
				Lcd.putChar(0,2,F("reading Source Caps"));
				main_state=menu_get_pd_data;
				MENU_DEBUG_PRINTLN2("reading Source Caps");
			}	// parameter done
			break;
		case menu_get_pd_data:
			if (controller->is_up())
			{	// PD Caps available
				MENU_DEBUG_PRINTLN2("menu_get_pd_data");
				main_state=menu_set_parameter;
				setOptions();
			}	// PD Caps available
			break;
		case menu_set_parameter:
			if (controller->is_power_ready())
			{
				MENU_DEBUG_PRINTLN2("menu_set_parameter");
				active_menu=menu_item_power;
				main_state=menu_set_parameter;
				Lcd.putChar(0,2,F("configuring PPS    "));
				main_state=menu_start;
				controller->enable_output(Parameter->getAutoOn());
				lastUpdate=menuTime;
			}
			break;
		case menu_start:
			main_state=menu_process;
			menu_mode=menu_mode_off;
			lock_item=menu_item_end;
			menu_item=0;
			lastAction=0;
			Lcd.SetBrightness(Parameter->getBrightness());
			force_update=true;
			
			switch (active_menu)
			{	// slect menu to show
				case menu_item_power:
					power_edit.uSet=controller->get_set_voltage();
					power_edit.iSet=controller->get_set_current();
					power_edit.uEdit=power_edit.uSet;
					power_edit.iEdit=power_edit.iSet;
					draw_menu(pgmTextPower,pgmIconsPower);
					break;
				case menu_item_profile:
					if (controller->get_profile_cnt()!=0)
					{
						draw_menu(pgmTextProfile,pgmIconsProfile);
						profile_edit.currentProfile=controller->get_current_profile();
						profile_edit.lastProfile=profile_edit.currentProfile;
					}
					else
					{
						main_state=menu_start;
						active_menu=menu_item_power;
					}
					break;
				case menu_item_settings:
					settings_edit.AutoOn=Parameter->getAutoOn();
					settings_edit.AutoSet=Parameter->getAutoSet();
					settings_edit.CVCC=(uint8_t)Parameter->getCVCC();
					settings_edit.Brightness=Parameter->getBrightness();
					draw_menu(pgmTextSettings,pgmIconsSettings);
					Lcd.ProgressBarInit(15,0,4,0);
					break;
				case menu_item_calibration:
				/* fall through */
				case menu_item_delete:
					//Parameter->getCalU(&calibration_edit.OutVoltage,&temp);
					Parameter->getCalI(	&calibration_edit.setCurrent.internal_value,
										&calibration_edit.setCurrent.reference_value);
					calibration_edit.editCurrent=calibration_edit.setCurrent;
					calibration_edit.measure=false;
					draw_menu(pgmTextCalibration,pgmIconsCalibration);
					break;
				default:
					active_menu=menu_item_power;
					main_state=menu_start;
					break;
			}	// slect menu to show
			break;
		case menu_process:
			switch (active_menu)
			{	// process active menu
				case menu_item_power:
					doPower();
					break;
				case menu_item_profile:
					doProfile();
					break;
				case menu_item_settings:
					doSettings();
					break;
				case menu_item_calibration:
					doCalibration();
					break;
				case menu_item_delete:
					doDelete();
					break;
				default:
					active_menu=menu_item_power;
					main_state=menu_start;
					break;
			}	// process active menu
			break;
		case menu_wait_pd_transition:
			if (controller->is_power_ready())
			{	// Power ok
				active_menu=menu_item_power;
				main_state=menu_start;
			}	// Power ok
			break;
		case menu_end:
		/* fall through */
		default:
			main_state=menu_start;
			break;
	} // switch (main_state)

	
	Lcd.process();
	button_process();
}	// process


void menu::draw_menu(const char * menuText, const menu_icon_et * iconList)
{	// draw menu
	uint8_t n;
	menu_icon_et icon;
	Lcd.Clear();
	Lcd.putChar_P(0,0,menuText);
	n=0;
	while ((icon=(menu_icon_et)pgm_read_byte(&iconList[n]))!=icon_cancel)
	{	// set icons
		Lcd.DefineChar_P(ICON_OFFSET+n,icon_tab[icon]);
		n++;
	}	// set icons
}	// draw menu

// process MenuTable, 
void menu::process_button(const menu_pos_st * menuPos)
{
	buttonState.buttonDown=button_down();
	buttonState.buttonHeld=button_held();
	buttonState.buttonPressed=button_pressed();
	buttonState.buttonTurns=button_turns();
	
	if (buttonState.buttonPressed)
	{	// button pressed
		if ((menu_item_et)pgm_read_byte(&menuPos[menu_item].menu_item)==menu_item_cancel)
		{
			buttonState.buttonPressed=false;
			buttonState.buttonDown=false;
			buttonState.buttonHeld=false;
			buttonState.buttonTurns=0;
			active_menu=menu_item_power;
			main_state=menu_start;
			menu_mode=menu_mode_off;
		}
	}	// button pressed
	
	if (buttonState.buttonDown || 
		buttonState.buttonHeld || 
		buttonState.buttonPressed  ||
		buttonState.buttonTurns)
	{
		lastAction=menuTime;
	}
		
	
	switch (menu_mode)
	{	// switch menu_mode
		case menu_mode_off:
			if (buttonState.buttonPressed || (buttonState.buttonTurns!=0))
			{	// activate menu
				menu_mode=menu_mode_select;
				menu_item=0;
				buttonState.buttonPressed=false;
				buttonState.buttonTurns=0;
			}	// menu activated
			break;
		case menu_mode_select:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				if ((menu_item_et)pgm_read_byte(&menuPos[menu_item].menu_item)<menu_item_menu_end)
				{	// switch between menus
					main_state=menu_start;
					menu_mode=menu_mode_off;
					active_menu=(menu_item_et)pgm_read_byte(&menuPos[menu_item].menu_item);
				}	// switch between menus
				else
				{	// switch to select edit mode
					menu_mode=(menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action);
					lock_item=(menu_item_et)pgm_read_byte(&menuPos[menu_item].lock_item);
				}	// switch to select edit mode
			}	// button pressed
			break;
		case menu_mode_select_edit:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				menu_mode=(menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action);
				lock_item=(menu_item_et)pgm_read_byte(&menuPos[menu_item].lock_item);
			}	// button pressed
			break;
		case menu_mode_select_edit_pos:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				menu_mode=(menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action);
				if (menu_mode==menu_mode_select_edit_pos)
					menu_mode=menu_mode_select_edit;
			}	// button pressed
			break;
		case menu_mode_edit:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				menu_mode=(menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action);
				if (menu_mode==menu_mode_edit)
						menu_mode=menu_mode_select_edit_pos;
			}	// button pressed
			break;
		default:
			menu_mode=menu_mode_off;
			break;
	}	// switch menu_mode

	next_menu_item(menuPos);
	
	if ((uint16_t)(menuTime-lastAction)>MENU_IDLE_TIME)
	{
		if (menu_mode==menu_mode_select)
			menu_mode=menu_mode_off;
	}

	set_cursor(pgm_read_byte(&menuPos[menu_item].x_pos),pgm_read_byte(&menuPos[menu_item].y_pos));
}

void menu::next_menu_item(const menu_pos_st * menuPos)
{	// process button turns
	bool bOk;
	if ((menu_mode<menu_mode_edit) && (buttonState.buttonTurns!=0))
	{	// Not in edit mode, use turns to move
		uint8_t menu_pos_cnt=menu_pos_items(menuPos);
		menu_item+=buttonState.buttonTurns;
		do
		{
			bOk=true;
			while (menu_item<0) menu_item+=menu_pos_cnt;
			while (menu_item>menu_pos_cnt) menu_item-=menu_pos_cnt+1;

			bOk=bOk && ((menu_item_et)pgm_read_byte(&menuPos[menu_item].menu_item)!=menu_item_end);

			bOk=bOk && ( ( (menu_mode==menu_mode_select_edit_pos) && 
						   ((menu_item_et)pgm_read_byte(&menuPos[menu_item].lock_item)==lock_item) ) ||
						 ( (menu_mode==menu_mode_select_edit) && 
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action)>menu_mode_select) &&
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action)!=menu_mode_edit) ) ||
						 ( (menu_mode==menu_mode_select) && 
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action)!=menu_mode_edit) &&
						   ((menu_item_et)pgm_read_byte(&menuPos[menu_item].lock_item)!=menu_item_end) )  );

			if (!bOk)
			{	// not suitabele, try next
				if (buttonState.buttonTurns>0)
					menu_item++;
				else
					menu_item--;
			}	// not suitabele, try next
		} while (!bOk);
		
	}	// Not in edit mode, use turns to move
}	// process button turns


void menu::set_cursor(uint8_t x, uint8_t y)
{	// set_curosr

	switch (menu_mode)
	{	// switch menu_mode
		case menu_mode_off:
			Lcd.CursorOn(false);
			Lcd.CursorBlink(false);
			break;
		case menu_mode_select:
			Lcd.CursorBlink(true);
			break;
		case menu_mode_select_edit:
			Lcd.CursorOn(true);
			Lcd.CursorBlink(false);
			break;
		case menu_mode_select_edit_pos:
			Lcd.CursorOn(true);
			Lcd.CursorBlink(true);
			break;
		case menu_mode_edit:
			Lcd.CursorOn(true);
			Lcd.CursorBlink(false);
			break;
	}	// switch menu_mode
	Lcd.CursorPos(x,y);
}	// set_curosr

uint8_t menu::menu_pos_items(const menu_pos_st * menuPos)
{	// menu_pos_items
	uint8_t n=0;
	
	while ((menu_item_et)pgm_read_byte(&menuPos[n].menu_item)!=menu_item_end) n++;
	return n;
} 	// menu_pos_items


void menu::doPower(void)
{	// do Power Menu
	uint16_t lastUpdateDelta=menuTime-lastUpdate;
	
	if (((lastUpdateDelta>(UPDATE_CYCLE)) && (menu_mode==menu_mode_off)) ||
		((lastUpdateDelta>(UPDATE_CYCLE_SET)) && (menu_mode!=menu_mode_off)) ||
		force_update)
	{	// update voltage and current readings
		// Update voltage & current settings and readings
		lastUpdate=menuTime;
		if (controller->is_constant_current_active())
			Lcd.putChar(19,0,F("!"));
		else
			Lcd.putChar(19,0,F(" "));

		// display settings independent from Menu
		if (force_update)
		{	// force_update
			PD_power_info_t power_info;
			controller->get_profile_info(controller->get_current_profile(),&power_info);

			Lcd.putChar(6,0,'1'+controller->get_current_profile());
			Lcd.putChar_P(8,0,controller->get_profile_text(power_info.type));

			if ((controller->get_operating_mode()!=controller_c::CONTROLLER_MODE_OFF) &&
				(controller->is_PPS()))
			{
				Lcd.putChar(18,3,0x07);
				switch (controller->get_operating_mode())
				{
					case controller_c::CONTROLLER_MODE_CV:
						Lcd.putChar(13,0,F("[U  ]"));
						break;
					case controller_c::CONTROLLER_MODE_CVCC:
						Lcd.putChar(13,0,F("[UI ]"));
						break;
					case controller_c::CONTROLLER_MODE_CVCCmax:
						Lcd.putChar(13,0,F("[UI^]"));
						break;
					default:
						Lcd.putChar(13,0,F("[   ]"));
						break;
				}
			}
			else
			{
				Lcd.putChar(18,3,0x02);
				Lcd.putChar(13,0,F("[   ]"));
			}
			force_update=false;
		}	// force_update

		Lcd.putFixed(5,2,6,3,3,controller->get_output_voltage_mV());
		Lcd.putFixed(13,2,6,3,3,controller->get_output_current_mA());
	}	// update voltage and current readings
	
	Lcd.putFixed(5,1,5,2,3,power_edit.uEdit);
	Lcd.putFixed(14,1,4,2,3,power_edit.iEdit);
		
	process_button(pgmMenuPower);
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuPower[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				power_edit.uSet=power_edit.uEdit;
				power_edit.iSet=power_edit.iEdit;
				
				if (Parameter->getAutoSet())
				{
					Parameter->setVoltage_mV(power_edit.uSet);
					Parameter->setCurrent_mA(power_edit.iSet);
					Parameter->write();
				}
				
				controller->set_power(power_edit.uSet,power_edit.iSet);
				menu_mode=menu_mode_off;
				menu_item=0;
			}	// button pressed
			break;
		case menu_item_cancel:
			if (buttonState.buttonPressed)
			{	// button pressed
				power_edit.uEdit=power_edit.uSet;
				power_edit.iEdit=power_edit.iSet;
			}	// button pressed
			break;
		case menu_item_activate1:
		case menu_item_activate2:
			if (buttonState.buttonPressed && (!controller->is_PPS())) 
			{
				menu_mode=menu_mode_off;
				active_menu=menu_item_profile;
				main_state=menu_start;
			}
			break;
		case menu_item_edit:
			if ((buttonState.buttonTurns) && (menu_mode==menu_mode_edit))
			{	// turns, edit
				switch (lock_item)
				{	// switch lock_item
					case menu_item_activate1:
						if (buttonState.buttonTurns>0)
							power_edit.uEdit+=pgm_read_word(&pgmMenuPower[menu_item].edit_val);
						else
							power_edit.uEdit-=pgm_read_word(&pgmMenuPower[menu_item].edit_val);
						if (power_edit.uEdit>controller->get_max_voltage_mV(power_edit.iEdit)) 
							power_edit.uEdit=controller->get_max_voltage_mV(power_edit.iEdit);
						if (power_edit.uEdit<controller->get_min_voltage_mV()) 
							power_edit.uEdit=controller->get_min_voltage_mV();
						break;
					case menu_item_activate2:
						if (buttonState.buttonTurns>0)
							power_edit.iEdit+=pgm_read_word(&pgmMenuPower[menu_item].edit_val);
						else
							power_edit.iEdit-=pgm_read_word(&pgmMenuPower[menu_item].edit_val);
						if (power_edit.iEdit>controller->get_max_current_mA(power_edit.uEdit)) 
							power_edit.iEdit=controller->get_max_current_mA(power_edit.uEdit);
						if (power_edit.iEdit<controller->get_min_current_mA()) 
							power_edit.iEdit=controller->get_min_current_mA();
						break;
					default:
						break;
				}	// switch lock_item
			}	// turns, edit
			break;
		case menu_item_switch:
			if (buttonState.buttonPressed)
			{	// toggle output switch
				controller->enable_output(!controller->is_output_enabled());
			}	// toggle output switch
		default:
			break;
	}	// switch menu_item
}	// do Power Menu

void menu::doProfile(void)
{	// Profile Menu
	if (force_update)
	{	// force_update
		controller->get_profile_info(profile_edit.currentProfile,&profile_edit.PowerInfo);
		Lcd.putUnsigned(3,0,1,profile_edit.currentProfile+1);
		Lcd.putUnsigned(7,0,1,controller->get_profile_cnt());
		Lcd.putChar_P(11,0,controller->get_profile_text(profile_edit.PowerInfo.type));

		Lcd.putFixed(4,2,4,2,3,profile_edit.PowerInfo.max_i);
		Lcd.putChar( 9,2,F("A"));

		if (profile_edit.PowerInfo.type==PD_PDO_TYPE_FIXED_SUPPLY)
		{	// fix, show min only
			Lcd.putChar(11,1,F("         "));
			Lcd.putFixed(3,1,5,2,3,profile_edit.PowerInfo.max_v);
		}
		else
		{	// pps, show min / max
			Lcd.putChar(11,1,'-');
			Lcd.putChar(19,1,'V');
			Lcd.putFixed(3,1,5,2,3,profile_edit.PowerInfo.min_v);
			Lcd.putFixed(13,1,5,2,3,profile_edit.PowerInfo.max_v);
			if (profile_edit.PowerInfo.type==PD_PDO_TYPE_BATTERY)
			{	// Battery profile, show max power
				Lcd.putChar( 9,2,F("W"));
				Lcd.putFixed(3,2,5,2,2,profile_edit.PowerInfo.max_p);
			}	// Battery profile, show max power
		}	// pps, show min / max
	}	// force_update
	process_button(pgmMenuProfile);
	
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuProfile[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				uint16_t u,i;
				MENU_DEBUG_PRINT2(menuTime);
				MENU_DEBUG_PRINT2(F(" doProfile:save U:"));
				u=controller->get_set_voltage();
				i=controller->get_set_current();

				MENU_DEBUG_PRINT2(profile_edit.PowerInfo.min_v);
				MENU_DEBUG_PRINT2(F("/"));
				MENU_DEBUG_PRINT2(u);
				MENU_DEBUG_PRINT2(F("/"));
				MENU_DEBUG_PRINT2(profile_edit.PowerInfo.max_v);


				MENU_DEBUG_PRINT2(F(" I:"));
				if (profile_edit.PowerInfo.type==PD_PDO_TYPE_FIXED_SUPPLY)
				{	// debug message fixed
					MENU_DEBUG_PRINT2(profile_edit.PowerInfo.max_i);
					MENU_DEBUG_PRINT2(F("/"));
					MENU_DEBUG_PRINT2(i);
					MENU_DEBUG_PRINT2(F("/"));
					MENU_DEBUG_PRINT2(profile_edit.PowerInfo.max_i);
				}	// debug message fixed
				else
				{	// debug message PPS
					MENU_DEBUG_PRINT2(controller->get_min_current_mA());
					MENU_DEBUG_PRINT2(F("/"));
					MENU_DEBUG_PRINT2(i);
					MENU_DEBUG_PRINT2(F("/"));
					MENU_DEBUG_PRINTLN2(controller->get_max_current_mA(u));
				}	// debug message PPS

				if (profile_edit.PowerInfo.type!=PD_PDO_TYPE_FIXED_SUPPLY)
				{	// pps, check limits
					if (u>profile_edit.PowerInfo.max_v)
						u=profile_edit.PowerInfo.max_v;
					if (u<profile_edit.PowerInfo.min_v)
						u=profile_edit.PowerInfo.min_v;
					if (i>controller->get_max_current_mA(u))
						i=controller->get_max_current_mA(u);
					if (i<controller->get_min_current_mA())
						i=controller->get_min_current_mA();
				}	// pps, check limits
				else
				{	// fixed profile, set
						u=profile_edit.PowerInfo.max_v;
						i=profile_edit.PowerInfo.max_i;
				}	// fixed profile, set
								
				if (Parameter->getAutoSet())
				{	// save setting for next start
						Parameter->setVoltage_mV(u);
						Parameter->setCurrent_mA(i);
						Parameter->setModePPS(
							profile_edit.PowerInfo.type !=
								PD_PDO_TYPE_FIXED_SUPPLY);
						Parameter->write();
				}	// save setting for next start
				
				if (profile_edit.PowerInfo.type == PD_PDO_TYPE_FIXED_SUPPLY)
					controller->set_voltage(u);
				else
					controller->set_power(u,i);

				menu_mode=menu_mode_off;
				menu_item=0;
				main_state=menu_wait_pd_transition;
			}	// button pressed
			break;
		case menu_item_radio1:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				if (buttonState.buttonTurns>0)
				{	// increment
					if (profile_edit.currentProfile<controller->get_profile_cnt()-1)
						profile_edit.currentProfile++;
				}	// increment
				else
				{	// decrement
					if (profile_edit.currentProfile>0)
						profile_edit.currentProfile--;
				}	// decrement
				force_update=true;
			}	// turns, edit
			break;
		default:
			break;
	}	// switch menu_item
}	// Profile Menu

void menu::doSettings(void)
{	// do Settings Menu
	
	if (force_update)
	{	// force_update
		if (settings_edit.AutoSet)
			Lcd.putChar(4,0,F("auto"));
		else
			Lcd.putChar(4,0,F("man "));
			
		if (settings_edit.AutoOn)
			Lcd.putChar(4,1,F("auto"));
		else
			Lcd.putChar(4,1,F("man "));

		switch ((controller_c::operating_mode_et)settings_edit.CVCC)
		{
			case controller_c::CONTROLLER_MODE_CV:
				Lcd.putChar(4,2,F("CV   "));
				break;
			case controller_c::CONTROLLER_MODE_CVCC:
				Lcd.putChar(4,2,F("CVCC "));
				break;
			case controller_c::CONTROLLER_MODE_CVCCmax:
				Lcd.putChar(4,2,F("CVCC^"));
				break;
			case controller_c::CONTROLLER_MODE_OFF:
			/* fall through */
			default:
				settings_edit.CVCC=(uint8_t)controller_c::CONTROLLER_MODE_OFF;
				Lcd.putChar(4,2,F("off  "));
				break;
		}

		// Brightness
		Lcd.ProgressSet((uint16_t)settings_edit.Brightness);
		force_update=false;
	}	// force_update

	process_button(pgmMenuSettings);
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuSettings[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				
				Parameter->setAutoOn(settings_edit.AutoOn);
				Parameter->setAutoSet(settings_edit.AutoSet);
				Parameter->setCVCC((controller_c::operating_mode_et)settings_edit.CVCC);
				Parameter->setBrightness(settings_edit.Brightness);
				Parameter->write();
				active_menu=menu_item_power;
				main_state=menu_start;
				menu_mode=menu_mode_off;
				menu_item=0;
				controller->set_operating_mode((controller_c::operating_mode_et)settings_edit.CVCC);
			}	// button pressed
			break;
		case menu_item_radio1:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				settings_edit.AutoSet=!settings_edit.AutoSet;
				force_update=true;
			}	// turns, edit
			break;
		case menu_item_radio2:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				settings_edit.AutoOn=!settings_edit.AutoOn;
				settings_edit.AutoSet|=settings_edit.AutoOn;
				force_update=true;
			}	// turns, edit
			break;
		case menu_item_radio3:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				settings_edit.CVCC+=buttonState.buttonTurns;
				force_update=true;
			}	// turns, edit
			break;
		case menu_item_radio4:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				if ((buttonState.buttonTurns>0) && 
					(settings_edit.Brightness<100))
					settings_edit.Brightness++;
				if ((buttonState.buttonTurns<0) && 
					(settings_edit.Brightness>0))
					settings_edit.Brightness--;
				Lcd.SetBrightness(settings_edit.Brightness);
				force_update=true;
			}	// turns, edit
			break;
		default:
			break;
	}	// switch menu_item

}	// do Settings Menu

void menu::doCalibration(void)
{	// do Calibration Menu
	uint16_t lastUpdateDelta=menuTime-lastUpdate;

	if ((calibration_edit.measure) && 
		((uint16_t)(menuTime-calibration_edit.Timer)>CAL_CYCLE))
	{	// update IIR Filter and editCurrent
		calibration_edit.Timer=menuTime;
		int16_t Current=controller->get_output_current_mA();
		if (Current<0) Current=0;
		calibration_edit.Filter.add(Current);
		calibration_edit.editCurrent.internal_value =
			calibration_edit.Filter.get();
	}	// update IIR Filter and editCurrent

	if (lastUpdateDelta>(UPDATE_CYCLE_SET) || force_update) 
	{	// update current readings
		Lcd.putFixed(12,1,5,3,3,calibration_edit.editCurrent.internal_value);
		Lcd.putFixed(12,2,5,3,3,calibration_edit.editCurrent.reference_value);
		lastUpdate=menuTime;
		force_update=false;
	}	// update current readings

	
	
	process_button(pgmMenuCalibration);
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuCalibration[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				menu_mode=menu_mode_off;
				menu_item=0;
				Parameter->setCalI(	calibration_edit.editCurrent.internal_value,
									calibration_edit.editCurrent.reference_value);
				Parameter->write();
				controller->set_output_current_calibration(
					calibration_edit.editCurrent.internal_value,
					calibration_edit.editCurrent.reference_value);
			}	// button pressed
			break;
		case menu_item_delete:
			if (buttonState.buttonPressed)
			{
				main_state=menu_process;
				Lcd.putChar(18,3,'\x04');
				menu_mode=menu_mode_select_edit;
				lock_item=menu_item_end;
				menu_item=0;
			}
			break;
		case menu_item_cancel:
			if (buttonState.buttonPressed)
			{
				controller->set_output_current_calibration(
					calibration_edit.setCurrent.internal_value,
					calibration_edit.setCurrent.reference_value);
			}
			break;
		case menu_item_activate1:
			if (buttonState.buttonPressed)
			{
				if (!calibration_edit.measure)
				{	// enter measure mode
					calibration_edit.measure=true;
					controller->reset_output_current_calibration();
					calibration_edit.Filter.set();
					calibration_edit.Timer=menuTime;
				}	// enter meauser mode
				else
				{	// leave meauser mode
					calibration_edit.measure=false;
				}	// leave meauser mode
			}
			break;
		case menu_item_edit:
			if ((buttonState.buttonTurns) && (menu_mode==menu_mode_edit))
			{	// turns, edit
				switch (lock_item)
				{	// switch lock_item
					case menu_item_activate1:
						if (buttonState.buttonTurns>0)
							if (CAL_LIMIT_I-calibration_edit.editCurrent.reference_value > 
								pgm_read_word(&pgmMenuCalibration[menu_item].edit_val)) 
								calibration_edit.editCurrent.reference_value += 
									pgm_read_word(&pgmMenuCalibration[menu_item].edit_val);
							else 
								calibration_edit.editCurrent.reference_value=CAL_LIMIT_I;
						else
							if (calibration_edit.editCurrent.reference_value >
								pgm_read_word(&pgmMenuCalibration[menu_item].edit_val)) 
								calibration_edit.editCurrent.reference_value -= 
									pgm_read_word(&pgmMenuCalibration[menu_item].edit_val);
							else 
								calibration_edit.editCurrent.reference_value=0;
						break;
					default:
						break;
				}	// switch lock_item
				force_update=true;
			}	// turns, edit
			break;
		default:
			break;
	}	// switch menu_item
}	// do Calibration Menu

void menu::doDelete(void)
{	// do Delete Menu
	process_button(pgmMenuDelete);
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuDelete[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				Parameter->clear(true);
				Lcd.ProgressBarInit(12,3,8,0);
				main_state=menu_read_parameter;
				menu_mode=menu_mode_off;
				set_cursor(0,0);
			}	// button pressed
			break;
		default:
			break;
	}	// switch menu_item
}	// do Delete Menu


void menu::setOptions(void)
{	// setOptions
	uint16_t Cal,Ref;
	uint16_t u;
	uint16_t i=5000;
	
	Parameter->getCalI(&Cal,&Ref);
	controller->set_output_current_calibration(Cal,Ref);
	
	//getLimits();
	
	if (Parameter->getAutoSet())
	{	// AutoSet
		// generate dummy profile change
		controller->set_operating_mode(Parameter->getCVCC());

		u=Parameter->getVoltage_mV();
		i=Parameter->getCurrent_mA();
		if (Parameter->getModePPS() && 				// reqeuested profile is APDO 
			(controller->get_max_voltage_mV(0)>0))	// max voltage is >0 if APDO is available
		{	// pps
			if (u>controller->get_max_voltage_mV(0))
				u=controller->get_max_voltage_mV(0);
			if (u<controller->get_min_voltage_mV())
				u=controller->get_min_voltage_mV();
			if (i>controller->get_max_current_mA(u))
				i=controller->get_max_current_mA(u);
			controller->set_power(u,i);
		}	// pps
		else
		{	// fix
			u=controller->get_fix_voltage_mV(u);
			controller->set_voltage(u);
		}	// fix
	} // AutoSet

	Lcd.SetBrightness(Parameter->getBrightness());
}	// setOptions
