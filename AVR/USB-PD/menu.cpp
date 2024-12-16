#ifdef ARDUINO_ARCH_AVR
	#include <avr/power.h>
#endif
#include "menu.h"
#include "button.h"


#ifdef ARDUINO_ARCH_AVR
	#define DEBUG_BAUD (230400)
#else
	#define DEBUG_BAUD (115200)
#endif

#define _menu_debug_level (2)	// 0: off
								// 1: regulator steps
								// 2: configuration
#if  _menu_debug_level == 1
	#define MENU_DEBUG_INIT(x) Serial1.begin(x)
	#define MENU_DEBUG_PRINT(x) Serial1.print(x)
	#define MENU_DEBUG_PRINTLN(x) Serial1.println(x)
	#define MENU_DEBUG_PRINT2(x)
	#define MENU_DEBUG_PRINTLN2(x)
#elif _menu_debug_level == 2
	#define MENU_DEBUG_INIT(x) Serial1.begin(x)
	#define MENU_DEBUG_PRINT(x) 
	#define MENU_DEBUG_PRINTLN(x) 
	#define MENU_DEBUG_PRINT2(x) Serial1.print(x)
	#define MENU_DEBUG_PRINTLN2(x) Serial1.println(x)
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
			/* currently not used
			{0x00,0x00,0x00,0x00,0x00,0x15,0x00,0x00}, // ...
			{0x00,0x04,0x02,0x1F,0x02,0x04,0x00,0x00}, // ->
			{0x00,0x04,0x08,0x1F,0x08,0x04,0x00,0x00}, // <- 
			{0x00,0x04,0x04,0x15,0x0E,0x04,0x00,0x00}, // dn
			{0x00,0x04,0x0E,0x15,0x04,0x04,0x00,0x00}, // up
			*/
													//	 01234567890123456789
const char menu::pgmTextStart[] PROGMEM=				" PD/PPS-Controller  "
														"===================="
														"Ver. :   3.3 nbl    "
														"Build:   " __DATE__;
			
													//   [.  .  .  .  .]  (.)
													//	 01234567890123456789
const char menu::pgmTextPower[] PROGMEM=				"Mode (x:...) [UI^] !" 
														"     UU.UU V  I.II A"
														"OUT  UU.UU V  I.II A"
														"[\x03  \x04  \x05  \x06  x]  (\x02)";

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
const char menu::pgmTextCalibration[] PROGMEM=			"I= I.III A:  I.III A"
														"                    "
														"                    "
														"[\x03  \x04  \x05  x]     (\x02)";


const char menu::pgmTextPowerProfileFIX[] PROGMEM = "FIX";
const char menu::pgmTextPowerProfileBAT[] PROGMEM = "BAT";
const char menu::pgmTextPowerProfileVAR[] PROGMEM = "VAR";
const char menu::pgmTextPowerProfileAUG[] PROGMEM = "AUG";
const char menu::pgmTextPowerProfileNON[] PROGMEM = "---";


 const menu::menu_icon_et menu::pgmIconsPower[] PROGMEM = {	icon_ramp, 
															icon_switch, 
															icon_flash, 
															icon_settings,
															icon_ok, 
															icon_regulator, 
															icon_cancel};
															
 const menu::menu_icon_et menu::pgmIconsProfile[] PROGMEM = {	icon_flash, 
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
		{	menu_item_activate1,	menu_item_activate1,menu_mode_select_edit_pos, 9, 0,     0},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			   3, 0,  1000},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			   5, 0,   100},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			   6, 0,    10},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			   7, 0,     1},
		{	menu_item_ok,			menu_item_end, 		menu_mode_select_edit,	   7, 3,     0},
		{	menu_item_cancel,		menu_item_end,		menu_mode_select_edit,	  10, 3,     0},
		{	menu_item_end,  		menu_item_end,		menu_mode_off,			   0, 0,     0}};

const menu::menu_pos_st menu::pgmMenuDelete[] PROGMEM =						
	{	{	menu_item_ok,			menu_item_end, 		menu_mode_select_edit,	   7, 3,     0},
		{	menu_item_cancel,		menu_item_end,		menu_mode_select_edit,	  10, 3,     0},
		{	menu_item_end,  		menu_item_end,		menu_mode_off,	   	   0, 0,     0}};


menu::menu(void)
{	// menu
	main_state=menu_init_start;
	RegulatorState.IntegralOffsetU=0;
	RegulatorState.RegulatorStep=regulator_off;
}	// menu

void menu::process(void)
{	// process
	static uint16_t temp;	
	switch (main_state)
	{	// switch (main_state)
		case menu_init_start:
			active_menu=menu_item_power;
			pinMode(POWER_LED_BLUE,OUTPUT);
			pinMode(POWER_LED_RED,OUTPUT);
			pinMode(POWER_LED_GREEN,OUTPUT);
			setPowerLED(led_power_off);
			
			digitalWrite(OUTPUT_SWITCH_PIN,LOW);
			pinMode(OUTPUT_SWITCH_PIN,OUTPUT);
			setOutput(false);
			
			button_init();
			PD_PPS.set_int_pin(FUSB302_INT_PIN);
			PD_PPS.set_fallback_power_option(PD_FALLBACK_POWER_OPTION);
			PD_PPS.init(FUSB302_INT_PIN,PD_POWER_OPTION_MAX_5V);
			#ifdef ARDUINO_ARCH_AVR
				PD_PPS.clock_prescale_set(2);
				clock_prescale_set(clock_div_2);
			#endif
			Lcd.init(20,4);
			Lcd.BacklightOn(true);
			Lcd.DisplayOn(true);
			Lcd.ProgressBarInit(0,3,20,0);
			Lcd.putChar_P(0,0,pgmTextStart);
			main_state=menu_init_delay;
			temp=PD_PPS.clock_ms();
			enableOutput(false);
			MENU_DEBUG_INIT(DEBUG_BAUD);
			MENU_DEBUG_PRINTLN(F("MENU_DEBUG_LEVEL 1"));
			MENU_DEBUG_PRINTLN2(F("MENU_DEBUG_LEVEL 2"));
			break;
		case menu_init_delay:
			if ((uint16_t)(PD_PPS.clock_ms()-temp)>INIT_DELAY)
			{
				Lcd.putChar(0,2,F("reading config  ..."));
				main_state=menu_read_parameter;
			}
			break;
		case menu_read_parameter:
			if (Parameter.process())
			{	// parameter busy
				Lcd.ProgressSet(Parameter.getProgress());
			}	// parameter busy
			else
			{	// parameter done
				Lcd.ProgressSet(0);
				main_state=menu_monitor_set_bus_voltage_range;
			}	// parameter done
			break;
		case menu_monitor_set_bus_voltage_range:
			monitor.setBusVoltageRange(ADC_BUS_VOLTAGE_RANGE);
			main_state=menu_monitor_set_bus_voltage_res;
			break;
		case menu_monitor_set_bus_voltage_res:
			monitor.setAdcAveraging(INA219::VOLTAGE,ADC_AVERAGE_SLOW);
			main_state=menu_monitor_set_shunt_voltage_res;
			break;
		case menu_monitor_set_shunt_voltage_res:
			monitor.setAdcAveraging(INA219::CURRENT,ADC_AVERAGE_SLOW);
			main_state=menu_monitor_set_shunt_gain;
			break;
		case menu_monitor_set_shunt_gain:
			monitor.setShuntGain(ADC_SHUNT_VOLTAGE_GAIN);
			main_state=menu_monitor_set_mode;
			break;
		case menu_monitor_set_mode:
			monitor.setOperationMode(INA219::MODE_ALL_CONTINOUS);
			main_state=menu_monitor_set_calibration;
			break;
		case menu_monitor_set_calibration:
			monitor.setCalibration(ADC_CALIBRATION_VALUE);
			Lcd.putChar(0,2,F("reading Source Caps"));
			main_state=menu_get_pd_data;
			break;
		case menu_get_pd_data:
			if (is_PD_ready())
			{	// PD Caps available
				main_state=menu_set_zero_offset;
				monitor.clear_ready_flag();
				getLimits();
			}	// PD Caps available
			break;
		case menu_set_zero_offset:
			Lcd.ProgressSet(0);
			active_menu=menu_item_power;
			main_state=menu_set_parameter;
			Lcd.putChar(0,2,F("configuring PPS    "));
			setOptions();
			main_state=menu_set_parameter;
			break;
		case menu_set_parameter:
			if (is_PD_ready() && monitor.conversion_ready(&temp))
			{
				main_state=menu_start;
				enableOutput(Parameter.getAutoOn());
				lastUpdate=PD_PPS.clock_ms();
			}
			break;
		case menu_start:
			main_state=menu_process;
			menu_mode=menu_mode_off;
			lock_item=menu_item_end;
			menu_item=0;
			lastAction=0;
			getCurrentProfile();
			enableRegulator();
//			Lcd.SetBrightness(Parameter.getBrightness());
			
			switch (active_menu)
			{	// slect menu to show
				case menu_item_power:
					if (RegulatorState.RegulatorStep==regulator_off)
					{
						power_edit.iSet=getBusCurrentSet();
						power_edit.uSet=getBusVoltageSet();
					}
					power_edit.uEdit=power_edit.uSet;
					power_edit.iEdit=power_edit.iSet;
					lastUpdate=PD_PPS.clock_ms()-UPDATE_CYCLE-1;
					draw_menu(pgmTextPower,pgmIconsPower);
					break;
				case menu_item_profile:
					if (profile_edit.maxProfile!=0)
					{
						draw_menu(pgmTextProfile,pgmIconsProfile);
					}
					else
					{
						main_state=menu_start;
						active_menu=menu_item_power;
					}
					break;
				case menu_item_settings:
					settings_edit.AutoOn=Parameter.getAutoOn();
					settings_edit.AutoSet=Parameter.getAutoSet();
					settings_edit.CVCC=(uint8_t)Parameter.getCVCC();
					settings_edit.Brightness=Parameter.getBrightness();
					draw_menu(pgmTextSettings,pgmIconsSettings);
					Lcd.ProgressBarInit(15,0,4,0);
					break;
				case menu_item_calibration:
				case menu_item_delete:
					Parameter.getCalU(&calibration_edit.OutVoltage,&temp);
					Parameter.getCalI(&calibration_edit.OutCurrent,&temp);
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
			Regulator();
			break;
		case menu_calibrating:
				if (monitor.conversion_ready(&temp))
				{
					Parameter.setCalI(calibration_edit.OutCurrent,getOutCurrent());
					Parameter.write();
					active_menu=menu_item_power;
					main_state=menu_set_zero_offset;
				}
			break;
		case menu_wait_pd_transition:
			if (is_PD_ready())
			{	// Power ok
				active_menu=menu_item_power;
				main_state=menu_start;
			}	// Power ok
			break;
		case menu_end:
		default:
			main_state=menu_start;
			break;
	} // switch (main_state)
	
	Lcd.process();
	button_process();
	Parameter.process();
	PD_PPS.run();
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
		lastAction=PD_PPS.clock_ms();
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
	
	if ((uint16_t)(PD_PPS.clock_ms()-lastAction)>MENU_IDLE_TIME)
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
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].lock_item)==lock_item) ) ||
						 ( (menu_mode==menu_mode_select_edit) && 
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action)>menu_mode_select) &&
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action)!=menu_mode_edit) ) ||
						 ( (menu_mode==menu_mode_select) && 
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].menu_action)!=menu_mode_edit) &&
						   ((menu_mode_et)pgm_read_byte(&menuPos[menu_item].lock_item)!=menu_item_end) )  );

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
	uint16_t temp;
	uint16_t lastUpdateDelta=PD_PPS.clock_ms()-lastUpdate;
	if (/*(RegulatorState.RegulatorStep!=regulator_off) && */
		(RegulatorState.RegulatorStep>=regulator_hold))
	{	// regulator is enabled output is inactive
		if (monitor.conversion_ready(&temp))
		{	// conversion Ready
			power_edit.uOut=temp;
			power_edit.iOut=getOutCurrent();
			power_edit.uOutAvg.add(power_edit.uOut);
			power_edit.iOutAvg.add(power_edit.iOut);
			monitor.clear_ready_flag();
		}	// conversion Ready
	}	// regulator is enabled output is inactive
	if (((lastUpdateDelta>(UPDATE_CYCLE)) && (menu_mode==menu_mode_off)) ||
		((lastUpdateDelta>(UPDATE_CYCLE_SET)) && (menu_mode!=menu_mode_off)))
	{	// update voltage and current readings
	
		if (RegulatorState.RegulatorStep!=regulator_off)
		{
			Lcd.putFixed(5,2,6,3,3,power_edit.uOutAvg.get());
			Lcd.putFixed(13,2,6,3,3,power_edit.iOutAvg.get());
		}
		else
		{
			Lcd.putFixed(5,2,6,3,3,power_edit.uOut);
			Lcd.putFixed(13,2,6,3,3,power_edit.iOut);
		}
		Lcd.putChar(6,0,'1'+getCurrentProfile());
		Lcd.putChar_P(8,0,getProfileText());
		lastUpdate=PD_PPS.clock_ms();
		if (RegulatorState.RegulatorStep!=regulator_off)
		{
			Lcd.putChar(18,3,0x07);
			switch (Parameter.getCVCC())
			{
				case parameter::regulator_mode_off:
					Lcd.putChar(13,0,F("[   ]"));
					break;
				case parameter::regulator_mode_cv:
					Lcd.putChar(13,0,F("[U  ]"));
					break;
				case parameter::regulator_mode_cvcc:
					Lcd.putChar(13,0,F("[UI ]"));
					break;
				case parameter::regulator_mode_cvcc_max:
					Lcd.putChar(13,0,F("[UI^]"));
					break;
			}
		}
		else
		{
			Lcd.putChar(18,3,0x02);
			Lcd.putChar(13,0,F("[   ]"));
		}
		if (RegulatorState.IntegralOffsetI!=0)
			Lcd.putChar(19,0,F("!"));
		else
			Lcd.putChar(19,0,F(" "));
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
				
				if (Parameter.getAutoSet())
				{
					savePDPPS(power_edit.uSet,power_edit.iSet);
				}
				
				setPDPPS(power_edit.uSet,power_edit.iSet);
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
			if (buttonState.buttonPressed && (profile_edit.PowerInfo.type==PD_PDO_TYPE_FIXED_SUPPLY)) 
				//( (profile_edit.PowerInfo.type==PD_PDO_TYPE_FIXED_SUPPLY)  ||
				//  (profile_edit.PowerInfo.type==PD_PDO_TYPE_NONE /*(enum PD_power_data_obj_type_t)(-1)*/) ) )
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
						if (power_edit.uEdit>profile_edit.PowerInfo.max_v) 
							power_edit.uEdit=profile_edit.PowerInfo.max_v;
						if (power_edit.uEdit<profile_edit.PowerInfo.min_v) 
							power_edit.uEdit=profile_edit.PowerInfo.min_v;
						break;
					case menu_item_activate2:
						if (buttonState.buttonTurns>0)
							power_edit.iEdit+=pgm_read_word(&pgmMenuPower[menu_item].edit_val);
						else
							power_edit.iEdit-=pgm_read_word(&pgmMenuPower[menu_item].edit_val);
						if (power_edit.iEdit>getProfileCurrentMax(power_edit.uSet)) 
							power_edit.iEdit=getProfileCurrentMax(power_edit.uSet);
						if (power_edit.iEdit<getBusCurrentMin()) power_edit.iSet=getBusCurrentMin();
						break;
					default:
						break;
				}	// switch lock_item
			}	// turns, edit
			break;
		case menu_item_switch:
			if (buttonState.buttonPressed)
			{	// toggle output switch
				enableOutput(!power_edit.output);
			}	// toggle output switch
		default:
			break;
	}	// switch menu_item
}	// do Power Menu

void menu::doProfile(void)
{	// Profile Menu
	Lcd.putUnsigned(3,0,1,profile_edit.currentProfile+1);
	Lcd.putUnsigned(7,0,1,profile_edit.maxProfile);
	Lcd.putChar_P(11,0,getProfileText());

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

	process_button(pgmMenuProfile);
	
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuProfile[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				uint16_t u,i;
				MENU_DEBUG_PRINT2(PD_PPS.clock_ms());
				MENU_DEBUG_PRINT2(F(" doProfile:save U:"));
				u=getBusVoltageSet();
				MENU_DEBUG_PRINT2(profile_edit.PowerInfo.min_v);
				MENU_DEBUG_PRINT2(F("/"));
				MENU_DEBUG_PRINT2(u);
				MENU_DEBUG_PRINT2(F("/"));
				MENU_DEBUG_PRINT2(profile_edit.PowerInfo.max_v);
				if (u>profile_edit.PowerInfo.max_v)
					u=profile_edit.PowerInfo.max_v;
				if (u<profile_edit.PowerInfo.min_v)
					u=profile_edit.PowerInfo.min_v;
				i=getBusCurrentSet();
				MENU_DEBUG_PRINT2(F(" I:"));
				MENU_DEBUG_PRINT2(getBusCurrentMin());
				MENU_DEBUG_PRINT2(F("/"));
				MENU_DEBUG_PRINT2(i);
				MENU_DEBUG_PRINT2(F("/"));
				MENU_DEBUG_PRINTLN2(getProfileCurrentMax(u));
				if (i>getProfileCurrentMax(u))
					i=getProfileCurrentMax(u);
				if (i<getBusCurrentMin())
					i=getBusCurrentMin();
				
				if (Parameter.getAutoSet())
				{	// save setting for next start
						savePDPPS(u,i);
				}	// save setting for next start
				
				setPDPPS(u,i);
				enableOutput(power_edit.output);
				menu_mode=menu_mode_off;
				menu_item=0;
			}	// button pressed
			break;
		case menu_item_radio1:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				if (buttonState.buttonTurns>0)
				{	// increment
					if (profile_edit.currentProfile<profile_edit.maxProfile-1)
						profile_edit.currentProfile++;
				}	// increment
				else
				{	// decrement
					if (profile_edit.currentProfile>0)
						profile_edit.currentProfile--;
				}	// decrement
				getProfile(profile_edit.currentProfile);
			}	// turns, edit
			break;
		default:
			break;
	}	// switch menu_item
}	// Profile Menu

void menu::doSettings(void)
{	// do Settings Menu
	if (settings_edit.AutoSet)
		Lcd.putChar(4,0,F("auto"));
	else
		Lcd.putChar(4,0,F("man "));
		
	if (settings_edit.AutoOn)
		Lcd.putChar(4,1,F("auto"));
	else
		Lcd.putChar(4,1,F("man "));

	switch ((parameter::regulator_mode_et)settings_edit.CVCC)
	{
		case parameter::regulator_mode_cv:
			Lcd.putChar(4,2,F("CV   "));
			break;
		case parameter::regulator_mode_cvcc:
			Lcd.putChar(4,2,F("CVCC "));
			break;
		case parameter::regulator_mode_cvcc_max:
			Lcd.putChar(4,2,F("CVCC^"));
			break;
		case parameter::regulator_mode_off:
		default:
			settings_edit.CVCC=(uint8_t)parameter::regulator_mode_off;
			Lcd.putChar(4,2,F("off  "));
			break;
	}

	// Brightness
	Lcd.ProgressSet((uint16_t)settings_edit.Brightness);

	process_button(pgmMenuSettings);
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuSettings[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				
				Parameter.setAutoOn(settings_edit.AutoOn);
				Parameter.setAutoSet(settings_edit.AutoSet);
				Parameter.setCVCC((parameter::regulator_mode_et)settings_edit.CVCC);
				Parameter.setBrightness(settings_edit.Brightness);
				Parameter.write();
				active_menu=menu_item_power;
				main_state=menu_start;
				menu_mode=menu_mode_off;
				menu_item=0;
				enableOutput(power_edit.output);
			}	// button pressed
			break;
		case menu_item_radio1:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				settings_edit.AutoSet=!settings_edit.AutoSet;
			}	// turns, edit
			break;
		case menu_item_radio2:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				settings_edit.AutoOn=!settings_edit.AutoOn;
				settings_edit.AutoSet|=settings_edit.AutoOn;
			}	// turns, edit
			break;
		case menu_item_radio3:
			if ((menu_mode==menu_mode_select_edit_pos) && 
				(buttonState.buttonTurns!=0))
			{	// turns, edit
				settings_edit.CVCC+=buttonState.buttonTurns;
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
			}	// turns, edit
			break;
		default:
			break;
	}	// switch menu_item

}	// do Settings Menu

void menu::doCalibration(void)
{	// do Calibration Menu
	uint16_t lastUpdateDelta=PD_PPS.clock_ms()-lastUpdate;
	if (lastUpdateDelta>(UPDATE_CYCLE_SET)) 
	{	// update current readings
		Lcd.putFixed(12,0,6,3,3,getOutCurrent());
		lastUpdate=PD_PPS.clock_ms();
	}	// update current readings

	Lcd.putFixed(3,0,5,3,3,calibration_edit.OutCurrent);
	
	process_button(pgmMenuCalibration);
	
	switch ((menu_item_et)pgm_read_byte(&pgmMenuCalibration[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				Lcd.ProgressBarInit(0,3,20,0);
				lastAction=PD_PPS.clock_ms();
				main_state=menu_calibrating;
				menu_mode=menu_mode_off;
				menu_item=0;
				monitor.setCalibration(ADC_CALIBRATION_VALUE);
				monitor.clear_ready_flag();
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
		case menu_item_edit:
			if ((buttonState.buttonTurns) && (menu_mode==menu_mode_edit))
			{	// turns, edit
				switch (lock_item)
				{	// switch lock_item
					case menu_item_activate1:
						if (buttonState.buttonTurns>0)
							if (CAL_LIMIT_I-calibration_edit.OutCurrent>pgm_read_word(&pgmMenuCalibration[menu_item].edit_val)) 
								calibration_edit.OutCurrent+=pgm_read_word(&pgmMenuCalibration[menu_item].edit_val);
							else 
								calibration_edit.OutCurrent=CAL_LIMIT_I;
						else
							if (calibration_edit.OutCurrent>pgm_read_word(&pgmMenuCalibration[menu_item].edit_val)) 
								calibration_edit.OutCurrent-=pgm_read_word(&pgmMenuCalibration[menu_item].edit_val);
							else 
								calibration_edit.OutCurrent=0;
						break;
					default:
						break;
				}	// switch lock_item
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
				Parameter.clear(true);
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


bool menu::is_PD_ready(void)
{	// is Pd ready
	bool bReady=false;
	switch (pd_transition_state)
	{
		case pd_transition_start:
			lastUpdate=PD_PPS.clock_ms();
			pd_transition_state=pd_transition_wait_transition;
			break;
		case pd_transition_wait_transition:
			if ((uint16_t)(PD_PPS.clock_ms()-lastUpdate)>PD_DELAY)
			{	// delay passed, check transition flags
				if (!PD_PPS.is_ps_transition())
				{
					lastUpdate=PD_PPS.clock_ms();
					pd_transition_state=pd_transition_wait_power;
				}
			}	// delay passed, check transition flags
			break;
		case pd_transition_wait_power:	
			if ((uint16_t)(PD_PPS.clock_ms()-lastUpdate)>PD_DELAY)
			{	// delay passed, check power flags
				if ((PD_PPS.is_power_ready() && !PD_PPS.is_PPS_ready()) ||
					(!PD_PPS.is_power_ready() && PD_PPS.is_PPS_ready()))
				{	// power ok, final delay
					lastUpdate=PD_PPS.clock_ms();
					pd_transition_state=pd_transition_wait_final;
				}	// power ok, final delay
			}	// delay passed, check power flags
			break;
		case pd_transition_wait_final:
			if ((uint16_t)(PD_PPS.clock_ms()-lastUpdate)>PD_DELAY)
			{	// delay passed, check power flags
				pd_transition_state=pd_transition_ready;
			}	// delay passed, check power flags
			break;
		case pd_transition_ready:
			bReady=true;
			break;
	}
	return bReady;
}	// is Pd ready 

void menu::setOptions(void)
{	// setOptions
	uint16_t Cal,Ref;
	uint32_t temp;
	uint16_t i=5000;
	
	Parameter.getCalI(&Cal,&Ref);
	temp=ADC_CALIBRATION_VALUE;
	temp*=Cal;
	temp/=Ref;
	monitor.setCalibration((uint16_t)temp);
	
	getLimits();
	
	if (Parameter.getAutoSet())
	{	// AutoSet
		// generate dummy profile change

		profile_edit.lastProfile=0;
		profile_edit.currentProfile=1;

		if (Parameter.getModePPS())
			profile_edit.PowerInfo.type=PD_PDO_TYPE_AUGMENTED_PDO;
		else
			profile_edit.PowerInfo.type=PD_PDO_TYPE_FIXED_SUPPLY;

		profile_edit.PowerInfo.max_v=Parameter.getVoltageStep();
		
		i=Parameter.getCurrentStep();
		
	} // AutoSet
	else
	{
		profile_edit.PowerInfo.max_v=5000;
	}

	if (Parameter.getCVCC()!=parameter::regulator_mode_off)
	{	// CV CVCC CVCC+Imax
		profile_edit.lastProfile=0;
		profile_edit.currentProfile=1;
		profile_edit.PowerInfo.type=PD_PDO_TYPE_AUGMENTED_PDO;
	}	// CV CVCC CVCC+Imax

	power_edit.uSet=profile_edit.PowerInfo.max_v;
	power_edit.iSet=i;

	if ((Parameter.getCVCC()!=parameter::regulator_mode_off) || Parameter.getAutoSet())
		setPDPPS(profile_edit.PowerInfo.max_v,i);

}	// setOptions

uint16_t menu::getBusVoltageSet(void)
{	// get currently set bus voltage
	return PD_PPS.get_voltage_mV();
}	// get currently set bus voltage

uint16_t menu::getBusCurrentSet(void)
{	// get max. available current
	return PD_PPS.get_current_mA();
}	// get max. available current

uint16_t menu::getBusCurrentMin(void)
{	// get min. available current
	return 100;
}	// get min. available current


uint16_t menu::getOutVoltage(void)
{	// get measured output voltage
	uint16_t BusVoltage_mV;
	monitor.conversion_ready(&BusVoltage_mV);
	return BusVoltage_mV;
}	// get measured output voltage

int16_t menu::getOutCurrent(void)
{	// get measured output current
	int16_t Current_mA=monitor.getCurrent();
	//if (Current_mA<0) Current_mA=-Current_mA;
	return Current_mA;
}	// get measured output current

uint16_t menu::setPDPPS(uint16_t Voltage, uint16_t Current)
{	// set new pd / pps profile / data
	MENU_DEBUG_PRINT2(PD_PPS.clock_ms());
	MENU_DEBUG_PRINT2(F(" setPDPPS: U="));
	MENU_DEBUG_PRINT2(Voltage);
	MENU_DEBUG_PRINT2(F(" I="));
	MENU_DEBUG_PRINT2(Current);
	Voltage/=20;
	Current/=50;
	if (profile_edit.lastProfile!=profile_edit.currentProfile)
	{	// change profile
		if (profile_edit.PowerInfo.type==PD_PDO_TYPE_FIXED_SUPPLY)
		{	// fix supply, fallback, just set mode
			if (profile_edit.PowerInfo.max_v==5000)
			{	// FIX 5V
				MENU_DEBUG_PRINTLN2(F(" FIX 5V"));
				PD_PPS.set_power_option(PD_POWER_OPTION_MAX_5V);
			}	// FIX 5V
			else if (profile_edit.PowerInfo.max_v==9000)
			{	// FIX 9V
				MENU_DEBUG_PRINTLN2(F(" FIX 9V"));
				PD_PPS.set_power_option(PD_POWER_OPTION_MAX_9V);
			}	// FIX 9V
			else if (profile_edit.PowerInfo.max_v==12000)
			{	// FIX 12V
				MENU_DEBUG_PRINTLN2(F(" FIX 12V"));
				PD_PPS.set_power_option(PD_POWER_OPTION_MAX_12V);
			}	// FIX 12V
			else if (profile_edit.PowerInfo.max_v==15000)
			{	// FIX 15V
				MENU_DEBUG_PRINTLN2(F(" FIX 15V"));
				PD_PPS.set_power_option(PD_POWER_OPTION_MAX_15V);
			}	// FIX 15V
			else if (profile_edit.PowerInfo.max_v==20000)
			{	// FIX 15V
				MENU_DEBUG_PRINTLN2(F(" FIX 20V"));
				PD_PPS.set_power_option(PD_POWER_OPTION_MAX_20V);
			}	// FIX 20V
			else
			{	// FIX 5V - Fallback
				MENU_DEBUG_PRINTLN2(F(" FIX 5V !"));
				PD_PPS.set_power_option(PD_POWER_OPTION_MAX_5V);
			}	// FIX 5V - Fallback
		}	// fix supply, fallback, just set mode
		else
		{	// change to PPS profile, reinit
			MENU_DEBUG_PRINTLN2(F(" INIT PPS"));
			PD_PPS.init_PPS(FUSB302_INT_PIN,Voltage,Current);
		}	// change to PPS profile, reinit
	}	// change profile
	else
	{ // no change in profile
		if (profile_edit.PowerInfo.type!=PD_PDO_TYPE_FIXED_SUPPLY)
		{	// set new voltage / current
			MENU_DEBUG_PRINTLN2(F(" SET PPS"));
			PD_PPS.set_PPS(Voltage,Current);
		}	// set new voltage / current
	} // no change in profile
	main_state=menu_wait_pd_transition;
	pd_transition_state=pd_transition_start;
	resetRegulator();
}	// set new pd / pps profile / data


uint16_t menu::enableOutput(bool bEnable)
{	// enable / disable output switch
	MENU_DEBUG_PRINT2(PD_PPS.clock_ms());
	MENU_DEBUG_PRINT2(F(" enableOutput="));
	MENU_DEBUG_PRINTLN2(bEnable);
	power_edit.output=bEnable;
	setOutput(bEnable);
	enableRegulator();
	if (!bEnable) 
		setPowerLED(led_power_off);	// outupt off -> LED off
	else if (profile_edit.PowerInfo.type!=PD_PDO_TYPE_AUGMENTED_PDO)
		setPowerLED(led_power_fix);	// no PPS -> mode "fix"
	else if (Parameter.getCVCC()!=parameter::regulator_mode_off)
		setPowerLED(led_power_cv);	// regulator on -> mode "CC"
	else
		setPowerLED(led_power_pps);	// otherwise just "pps" mode
		
}	// enable / disable output switch

void menu::setPowerLED(led_power_et led_mode)
{	// set power LED
	switch (led_mode)
	{	// switch led_mode
		case led_power_cc:
			MENU_DEBUG_PRINTLN("PowerLED: CC");
			MENU_DEBUG_PRINTLN2(F("PowerLED: CC"));
			digitalWrite(POWER_LED_BLUE,POWER_LED_OFF);
			digitalWrite(POWER_LED_GREEN,POWER_LED_OFF);
			digitalWrite(POWER_LED_RED,!POWER_LED_OFF);
			break;
		case led_power_cv:
			MENU_DEBUG_PRINTLN("PowerLED: CV");
			MENU_DEBUG_PRINTLN2(F("PowerLED: CV"));
			digitalWrite(POWER_LED_BLUE,!POWER_LED_OFF);
			digitalWrite(POWER_LED_GREEN,POWER_LED_OFF);
			digitalWrite(POWER_LED_RED,POWER_LED_OFF);
			break;
		case led_power_fix:
			MENU_DEBUG_PRINTLN("PowerLED: FIX");
			MENU_DEBUG_PRINTLN2(F("PowerLED: FIX"));
			digitalWrite(POWER_LED_BLUE,POWER_LED_OFF);
			digitalWrite(POWER_LED_GREEN,!POWER_LED_OFF);
			digitalWrite(POWER_LED_RED,POWER_LED_OFF);
			break;
		case led_power_pps:
			MENU_DEBUG_PRINTLN("PowerLED: PPS");
			MENU_DEBUG_PRINTLN2(F("PowerLED: PPS"));
			digitalWrite(POWER_LED_BLUE,POWER_LED_OFF);
			digitalWrite(POWER_LED_GREEN,!POWER_LED_OFF);
			digitalWrite(POWER_LED_RED,!POWER_LED_OFF);
			break;
		case led_power_off:
		default:
			MENU_DEBUG_PRINTLN("PowerLED: off");
			MENU_DEBUG_PRINTLN2(F("PowerLED: off"));
			digitalWrite(POWER_LED_BLUE,POWER_LED_OFF);
			digitalWrite(POWER_LED_GREEN,POWER_LED_OFF);
			digitalWrite(POWER_LED_RED,POWER_LED_OFF);
			break;		
	}	// switch led_mode
}	// set power LED

void menu::setOutput(bool bEnable)
{	// setOutput
	digitalWrite(OUTPUT_SWITCH_PIN,bEnable);
}	// setOutput


void menu::Regulator(void)
{	// regulator
	uint16_t temp;
	switch (RegulatorState.RegulatorStep)
	{	// regulator state machine
		case regulator_start:
			if (PD_PPS.is_PPS_ready())
			{
				MENU_DEBUG_PRINT(F(" R="));
				MENU_DEBUG_PRINT(PD_PPS.clock_ms());
				RegulatorState.Timer=PD_PPS.clock_ms();
				RegulatorState.RegulatorStep=regulator_wait_delay;
			}
			break;
		case regulator_wait_delay:			
			if ((uint16_t)(PD_PPS.clock_ms()-RegulatorState.Timer)>REG_DELAY)
			{
				MENU_DEBUG_PRINT(F(" D="));
				MENU_DEBUG_PRINT(PD_PPS.clock_ms());
				monitor.clear_ready_flag();
				RegulatorState.RegulatorStep=regulator_measure;
			}
			break;
		case regulator_measure:
			if (monitor.conversion_ready(&temp))
			{
				MENU_DEBUG_PRINT(F(" M="));
				MENU_DEBUG_PRINTLN(PD_PPS.clock_ms());
				power_edit.uOut=temp;
				power_edit.iOut=getOutCurrent();
				power_edit.uOutAvg.add(power_edit.uOut);
				power_edit.iOutAvg.add(power_edit.iOut);
				if (power_edit.iOut<0) power_edit.iOut=0;
				
				MENU_DEBUG_PRINT(F(" U="));
				MENU_DEBUG_PRINT(power_edit.uOut);
				MENU_DEBUG_PRINT(F(" I="));
				MENU_DEBUG_PRINT(power_edit.iOut);
				
				RegulatorState.RegulatorStep=regulator_start;
				if (Parameter.getCVCC()!=parameter::regulator_mode_cv)
				{	// CV mode / PPS Supply ist used for current limiting
					if (!RegulatorCC(power_edit.iSet-power_edit.iOut))
					{	// not in current limiting mode
						RegulatorCV(power_edit.uSet-power_edit.uOut);
					}	// not in current limiting mode
				}	// CV mode / PPS Supply ist used for current limiting
				else
				{	// CV only
					if (power_edit.iOut<power_edit.iSet)
					{
						if (!RegulatorState.LockCount) 
						{	// not locked -> regulator calculation
							RegulatorCV(power_edit.uSet-power_edit.uOut);
							// if Regulator does not need to set new value,
							// LED will not be changed, need to be set seperately
							setPowerLED(led_power_cv);	
						}	// not locked -> regulator calculation
						else
						{	// locked -> wait
							MENU_DEBUG_PRINT(F(" CW="));
							MENU_DEBUG_PRINT(RegulatorState.LockCount);
							RegulatorState.LockCount--;
						}	// locked -> wait
					}
					else
					{	// set lock count
						setPowerLED(led_power_cc);	
						RegulatorState.LockCount=CURRENT_LOCK_CYCLES;
					}	// set lock count
				}	// CV only
			}
			break;
		case regulator_wait_transition_start:
			if(PD_PPS.is_ps_transition())
			{
				RegulatorState.RegulatorStep=regulator_wait_transition_end;
				MENU_DEBUG_PRINT(F(" T="));
				MENU_DEBUG_PRINT(PD_PPS.clock_ms());
			}
			break;
		case regulator_wait_transition_end:
			if(!PD_PPS.is_ps_transition())
			{
				RegulatorState.RegulatorStep=regulator_start;
				MENU_DEBUG_PRINT(F(" E="));
				MENU_DEBUG_PRINT(PD_PPS.clock_ms());
			}
			break;
		default:
			// inactive, do nothing
			break;
	}	// regulator state machine
}	// regulator

bool menu::RegulatorCC(int16_t delta)
{	// calculate constant current
	int8_t		StepSign;
	uint8_t		Scale;
	uint16_t	relStep;
	uint16_t	lastDelta;
	int16_t		OffsetLimit;
	if ((delta<0) || 							// current limit exceedet or
		(RegulatorState.IntegralOffsetI!=0))	// allreaedy in current limit mode
	{	// start constant current regulation
		if (abs(delta)>RegulatorState.min_deltaI)
		{	// current deviation to high
			MENU_DEBUG_PRINT(F(" dI="));
			MENU_DEBUG_PRINT(delta);
			MENU_DEBUG_PRINT(F(" iU="));
			MENU_DEBUG_PRINT(RegulatorState.IntegralOffsetU);
			MENU_DEBUG_PRINT(F(" iI="));
			MENU_DEBUG_PRINT(RegulatorState.IntegralOffsetI);
		
			// clear min_delta in order to recalculate
			if (RegulatorState.IntegralStepI==0)
				RegulatorState.min_deltaI=0;
			
			// calculate relative step size
			Scale=1;
			while ((power_edit.iSet>>Scale)>abs(delta)) Scale++;
			relStep=power_edit.uSet>>Scale;

			MENU_DEBUG_PRINT(F(" Scale="));
			MENU_DEBUG_PRINT(Scale);
			MENU_DEBUG_PRINT(F(" relStep="));
			MENU_DEBUG_PRINT(relStep);
			
			// save sign and preserve positive step size
			StepSign=(RegulatorState.IntegralStepI>0)?(1):((RegulatorState.IntegralStepI<0)?(-1):(0));
			RegulatorState.IntegralStepI=abs(RegulatorState.IntegralStepI);
			
			if (((StepSign>0) && (delta<0)) ||	// last step increment and current to high or
				((StepSign<0) && (delta>0)))	// last step decrement amd current to low
			{	// change in direction
				
				// check if stepsize ist already minimum
				if (RegulatorState.IntegralStepI==MIN_INCREMENT_U)
				{	// oscillation at min increment
					RegulatorState.OscillationCount++;
					RegulatorState.min_deltaI_sum+=abs(delta);
				}	// oscillation at min increment
				else
				{	// reset oscillation count
					RegulatorState.OscillationCount=0;
					RegulatorState.min_deltaI_sum=0;
				}	// reset oscillation count

				// reduce IntegralStepI
				RegulatorState.CycleCount=0;
				RegulatorState.IntegralStepI>>=1;
			}	// change in direction
				
			// check if several corrrections in one direction
			if (RegulatorState.CycleCount>INCREMENT_LIMIT)
			{
				RegulatorState.CycleCount=0;
				RegulatorState.IntegralStepI<<=1;
			}			
			
			// check if delta has been doubled since last cycle
			if (delta<0) 
				lastDelta=RegulatorState.lastNegDeltaI;
			else
				lastDelta=RegulatorState.lastPosDeltaI;

			MENU_DEBUG_PRINT(F(" ldI="));
			MENU_DEBUG_PRINT(lastDelta);
			
			if ((lastDelta!=0) && 				// value present
				((lastDelta<<1)<abs(delta)))	// and change of +100%
				RegulatorState.IntegralStepI=0;	// clear step Size to force relStep

			if (delta<0) 
				RegulatorState.lastNegDeltaI=abs(delta);
			else
				RegulatorState.lastPosDeltaI=abs(delta);

			if ((RegulatorState.IntegralStepI==0) ||
				(relStep<RegulatorState.IntegralStepI))
			{	// use minimum of current tepsize and relative stepsize
				RegulatorState.IntegralStepI=relStep;
			}	// use minimum of current tepsize and relative stepsize

			if (RegulatorState.IntegralStepI<MIN_INCREMENT_U) 
			{	// check minium increment
				RegulatorState.IntegralStepI=MIN_INCREMENT_U;
			}	// check minium increment
							
			// restore sign
			if (delta<0)
				RegulatorState.IntegralStepI=
					-RegulatorState.IntegralStepI;
							
			// calculate new integral offset
			RegulatorState.IntegralOffsetI+=RegulatorState.IntegralStepI;
			RegulatorState.CycleCount++;
							
			OffsetLimit=profile_edit.PowerLimits.min_v;
			OffsetLimit-=power_edit.uSet+RegulatorState.IntegralOffsetU;
							
			if (RegulatorState.IntegralOffsetI<OffsetLimit)
			{	// Voltage Reduction to high, start fuse counter
				RegulatorState.FuseCount++;
				RegulatorState.IntegralOffsetI=OffsetLimit;	
				if (RegulatorState.FuseCount>=FUSE_LIMIT)
				{	// fuse limit reached, turn off power
					enableOutput(false);
				}	// fuse limit reached, turn off power
			}	// Voltage Reduction to high, start fuse counter
			else
			{	// reset fuse counter
				RegulatorState.FuseCount=0;
			}	// reset fuse counter
			
			if (RegulatorState.IntegralOffsetI>0)
			{	// out of current limiting, reset current regulator
				RegulatorState.FuseCount=0;
				RegulatorState.CycleCount=0;
				RegulatorState.IntegralStepI=0;
				RegulatorState.IntegralOffsetI=0;
				RegulatorState.OscillationCount=0;
				RegulatorState.min_deltaI=0;
				RegulatorState.min_deltaI_sum=0;
				RegulatorState.lastNegDeltaI=0;
				RegulatorState.lastPosDeltaI=0;
			}	// out of current limiting, reset current regulator

			if (RegulatorState.OscillationCount>MAX_OSC_I) 
			{	// allready several oscillations at MIN_INCREMENT_U
				// calculate tolerance window for current
				RegulatorState.min_deltaI+=RegulatorState.min_deltaI_sum<<1;
				RegulatorState.min_deltaI/=RegulatorState.OscillationCount;
				RegulatorState.min_deltaI_sum=0;
				RegulatorState.FuseCount=0;
				RegulatorState.CycleCount=0;
				RegulatorState.IntegralStepI=0;
				RegulatorState.OscillationCount=0;
				if (RegulatorState.min_deltaI<MIN_DELTA_I)
					RegulatorState.min_deltaI=MIN_DELTA_I;
				RegulatorState.lastNegDeltaI=0;
				RegulatorState.lastPosDeltaI=0;
			}	// allready several oscillations at MIN_INCREMENT_U

			MENU_DEBUG_PRINT(F(" Istep="));
			MENU_DEBUG_PRINT(RegulatorState.IntegralStepI);
			MENU_DEBUG_PRINT(F(" iI="));
			MENU_DEBUG_PRINT(RegulatorState.IntegralOffsetI);
			MENU_DEBUG_PRINT(F(" osc="));
			MENU_DEBUG_PRINT(RegulatorState.OscillationCount);
			MENU_DEBUG_PRINT(F(" msum="));
			MENU_DEBUG_PRINT(RegulatorState.min_deltaI_sum);
			MENU_DEBUG_PRINT(F(" minD="));
			MENU_DEBUG_PRINT(RegulatorState.min_deltaI);
			MENU_DEBUG_PRINT(F(" Cycle="));
			MENU_DEBUG_PRINT(RegulatorState.CycleCount);
			MENU_DEBUG_PRINT(F(" Fuse="));
			MENU_DEBUG_PRINTLN(RegulatorState.FuseCount);

			
			RegulatorSet();
		}	// current deviation to high
		return true;
	}	// start constant current regulation
	else
		return false; 
}	// calculate constant current

bool menu::RegulatorCV(int16_t delta)
{	// calculate constant voltage
	int16_t OffsetLimit;
	if (abs(delta)>MIN_DELTA_U)
	{	// voltage difference , increase Setting
		if ( (abs(RegulatorState.IntegralOffsetU)<MAX_INTEGRAL_U) ||	// not yet saturated
			 ( (RegulatorState.IntegralOffsetU>0) && (delta<0) ) || 	// overcompensated 
			 ( (RegulatorState.IntegralOffsetU<0) && (delta>0) ) )		// undercompensated
		{	// Integral Offset within Limits
			RegulatorState.IntegralOffsetU+=delta;
			
			// max. Offset to lowest possible voltage
			OffsetLimit=profile_edit.PowerLimits.min_v;
			OffsetLimit-=power_edit.uSet;
			if (RegulatorState.IntegralOffsetU<OffsetLimit)
			{	// new offset sets voltage below min voltage
				RegulatorState.IntegralOffsetU=OffsetLimit;
			}	// new offset sets voltage below min voltage
			
			// max. Offset to highest possible voltage
			OffsetLimit=profile_edit.PowerLimits.max_v;
			OffsetLimit-=power_edit.uSet;
			if (RegulatorState.IntegralOffsetU>OffsetLimit)
			{	// new offset sets voltage below min voltage
				RegulatorState.IntegralOffsetU=OffsetLimit;
			}	// new offset sets voltage below min voltage
			
			MENU_DEBUG_PRINT(F(" dU="));
			MENU_DEBUG_PRINT(delta);
			MENU_DEBUG_PRINT(F(" iU="));
			MENU_DEBUG_PRINT(RegulatorState.IntegralOffsetU);

			RegulatorSet();
		}	// Integral Offset within Limits
		else
		{	// saturated, just wait
		}	// saturated, just wait
		return true;
	}	// voltage difference , increase Setting
	else
		return false;
}	// calculate constant voltage

void menu::RegulatorSet(void)
{	// set new voltage
	uint16_t uPPS;
	uint16_t iPPS;
	if (power_edit.output)
	{	// power is active, set new value
		MENU_DEBUG_PRINT2(PD_PPS.clock_ms());
		MENU_DEBUG_PRINT2(F(" RegulatorSet: "));
		uPPS=power_edit.uSet;
		uPPS+=RegulatorState.IntegralOffsetU;
		uPPS+=RegulatorState.IntegralOffsetI;
		if (uPPS!=RegulatorState.lastVoltageSet)
		{	// set only if different to last value
			RegulatorState.lastVoltageSet=uPPS;
			uPPS=uPPS/20;
			iPPS=power_edit.iSet;
			iPPS/=50;
			if (Parameter.getCVCC()!=parameter::regulator_mode_cvcc_max)
			{	// use power supply for additional current limiting
				MENU_DEBUG_PRINT2(F("Ustep="));
				MENU_DEBUG_PRINT2(uPPS);
				MENU_DEBUG_PRINT2(F(" Istep="));
				MENU_DEBUG_PRINT2(iPPS);
				PD_PPS.set_PPS(uPPS,iPPS);
			}	// use power supply for additional current limiting
			else
			{	// set max current, to disable current limiting from power supply
				PD_PPS.set_PPS(uPPS,100);	// TODO
				MENU_DEBUG_PRINT2(F("Ustep="));
				MENU_DEBUG_PRINT2(uPPS);
				MENU_DEBUG_PRINT2(F(" Istep=max"));
			}
			RegulatorState.RegulatorStep=regulator_wait_transition_start;
			MENU_DEBUG_PRINT(F(" S="));
			MENU_DEBUG_PRINT(PD_PPS.clock_ms());
		}	// set only if different to last value
		if (RegulatorState.IntegralOffsetI)
			setPowerLED(led_power_cc);
		else
			setPowerLED(led_power_cv);
	}	// power is active, set new value
}	// set new voltage

void menu::enableRegulator(void)
{	// enable Regulator
	MENU_DEBUG_PRINT2(PD_PPS.clock_ms());
	MENU_DEBUG_PRINT2(F(" enable Regulator: "));
	if ((Parameter.getCVCC()==parameter::regulator_mode_off) ||
		(profile_edit.PowerInfo.type!=PD_PDO_TYPE_AUGMENTED_PDO))
	{	// not activated nor suitable profile->off
		MENU_DEBUG_PRINT2(F("off, ADC slow"));
		RegulatorState.RegulatorStep=regulator_off;
		RegulatorState.IntegralOffsetU=0;
		monitor.setAdcAveraging(INA219::VOLTAGE,ADC_AVERAGE_SLOW);
		monitor.setAdcAveraging(INA219::CURRENT,ADC_AVERAGE_SLOW);
	}	// not activated nor suitable profile->off
	else
	{	// activated & suitable profile selected
		MENU_DEBUG_PRINT2(F("ADC fast "));
		monitor.setAdcAveraging(INA219::VOLTAGE,ADC_AVERAGE_REGULATOR);
		monitor.setAdcAveraging(INA219::CURRENT,ADC_AVERAGE_REGULATOR);
		switch (RegulatorState.RegulatorStep)
		{	// decision depending on current State
			case regulator_start:
			case regulator_measure:
			case regulator_wait_transition_start:
			case regulator_wait_transition_end:
			case regulator_wait_delay:			
				// if active: no Output -> just disable
				if (!power_edit.output)
				{
					MENU_DEBUG_PRINT2(F(" disable"));
					resetRegulator();
					RegulatorState.RegulatorStep=regulator_hold;
				}
				break;
			case regulator_off:
				MENU_DEBUG_PRINT2(F(" off -> hold"));
				RegulatorState.RegulatorStep=regulator_hold;
			case regulator_hold:
				// if inactive active: Output on -> just enable
				if (power_edit.output)
				{
					MENU_DEBUG_PRINT2(F(" enable"));
					resetRegulator();
					// get last settings
					RegulatorState.lastVoltageSet=getBusVoltageSet();
					RegulatorState.IntegralOffsetU=RegulatorState.lastVoltageSet;
					RegulatorState.IntegralOffsetU-=power_edit.uSet;
				}
				break;
			default:
				RegulatorState.RegulatorStep=regulator_off;
				break;
		}	// decision depending on current State
	}	// activated & suitable profile selected
	MENU_DEBUG_PRINTLN2();
}	// enable Regulator

void menu::resetRegulator(void)
{	// reset Regulator to start
	RegulatorState.RegulatorStep=regulator_start;
	RegulatorState.IntegralOffsetU=0;
	RegulatorState.IntegralOffsetI=0;
	RegulatorState.IntegralStepI=0;
	RegulatorState.CycleCount=0;
	RegulatorState.FuseCount=0;
	RegulatorState.OscillationCount=0;
	RegulatorState.LockCount=0;
	RegulatorState.min_deltaI=0;
	RegulatorState.min_deltaI_sum=0;
	RegulatorState.lastNegDeltaI=0;
	RegulatorState.lastPosDeltaI=0;
	RegulatorState.lastVoltageSet=0;
}	// reset Regulator to start

void menu::savePDPPS(uint16_t Voltage, uint16_t Current)
{	// savePDPPS
	Parameter.setModePPS(profile_edit.PowerInfo.type!=PD_PDO_TYPE_FIXED_SUPPLY);
	if (profile_edit.PowerInfo.type!=PD_PDO_TYPE_FIXED_SUPPLY)
	{	// PPS mode
		Parameter.setVoltageStep(Voltage);
		Parameter.setCurrentStep(Current);
	}	// PPS mode
	else
	{	// Fiexed mode
		Parameter.setVoltageStep(profile_edit.PowerInfo.max_v);
		Parameter.setCurrentStep(profile_edit.PowerInfo.max_i);
	}	// Fiexed mode
	Parameter.write();
}	// savePDPPS

uint8_t menu::getCurrentProfile(void)
{	// getCurrentProfile
	getProfileCnt();
	getProfile(PD_PPS.get_selected_PDO());
	profile_edit.lastProfile=profile_edit.currentProfile;
	return profile_edit.currentProfile;
}	// getCurrentProfile

uint8_t menu::getProfileCnt(void)
{	// getProfileCnt
	profile_edit.maxProfile=0; 
	while (PD_PPS.get_PDO_info(profile_edit.maxProfile,&profile_edit.PowerInfo))
		profile_edit.maxProfile++;
}	// getProfileCnt

void menu::getProfile(uint8_t n) // get specific profile data
{	//	getProfile
	profile_edit.currentProfile=n;
	//profile_edit.PowerInfo.type=PD_PDO_TYPE_NONE /*-1*/;   /* Added for none PD 5V Supplies */


	PD_PPS.get_PDO_info(n,&profile_edit.PowerInfo);
	/*
	profile_edit.PowerInfo.min_v*=50; // mV
	profile_edit.PowerInfo.max_v*=50; // mV
	profile_edit.PowerInfo.max_p*=25; // 10mW
	profile_edit.PowerInfo.max_i*=10; // mA
	*/
}	//	getProfile

void menu::getLimits(void)
{	// getLimits
	getProfileCnt();
	profile_edit.PowerLimits.max_i=0;
	profile_edit.PowerLimits.max_v=0;
	profile_edit.PowerLimits.min_v=20000;
	for (uint8_t n=0; n<profile_edit.maxProfile; n++)
	{
		getProfile(n);
		if (profile_edit.PowerInfo.max_i>profile_edit.PowerLimits.max_i)
			profile_edit.PowerLimits.max_i=profile_edit.PowerInfo.max_i;
		if (profile_edit.PowerInfo.max_v>profile_edit.PowerLimits.max_v)
			profile_edit.PowerLimits.max_v=profile_edit.PowerInfo.max_v;
		if ((profile_edit.PowerInfo.min_v>0) &&
			(profile_edit.PowerInfo.min_v<profile_edit.PowerLimits.min_v))
			profile_edit.PowerLimits.min_v=profile_edit.PowerInfo.min_v;
	}
}	// getLimits

const char * menu::getProfileText(void) // get text of selected profile
{	// getProfileText
	switch (profile_edit.PowerInfo.type)
	{	// switch Profile type
		case PD_PDO_TYPE_FIXED_SUPPLY:
			return pgmTextPowerProfileFIX;
			break;
		case PD_PDO_TYPE_BATTERY:
			return pgmTextPowerProfileBAT;
			break;
		case PD_PDO_TYPE_VARIABLE_SUPPLY:
			return pgmTextPowerProfileVAR;
			break;
		case PD_PDO_TYPE_AUGMENTED_PDO:
			return pgmTextPowerProfileAUG;
			break;
		default:           
			return pgmTextPowerProfileNON;
			break;          
	}	// switch Profile type
}	// getProfileText
	

uint16_t menu::getProfileCurrentMax(uint16_t mV)
{	// getProfileCurrentMax
	uint8_t a=0;
	uint8_t b=0;
	uint16_t maxP=profile_edit.PowerInfo.max_p;
	uint16_t maxI=profile_edit.PowerInfo.max_i;
	
	if (profile_edit.PowerInfo.type==PD_PDO_TYPE_BATTERY)
	{	// calculate max current
		maxP=profile_edit.PowerInfo.max_p;
		a=0;b=0;
		while (maxP<6553) { maxP*=10; a++; }
		while (mV%10==0) { mV/=10; b++; }
		maxI=maxP/mV;
		a+=b+1; // one time more because power is in 10 mW
		while (a>0) {maxI*=10; a--;}
		if (maxI>profile_edit.PowerInfo.max_i)
			maxI=profile_edit.PowerInfo.max_i;
	}	// calculate max current

	return maxI;
}	// getProfileCurrentMax

