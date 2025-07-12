#include "Arduino.h"
#include "menu_mini.h"
#include "../button/rotary_button.h"
#include "../../messages.h"
#include "../tool/convert.h"
#define DEBUG_BAUD (115200)

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

													//	 01234567890123
const char menu_mini_c::pgmTextStart[] =				"    PD/PPS    "
														"  Controller  "
														"     x.y      "
														" mon dd yyyy  ";

													//   . . . . .    .
													//	 01234567890123
const char menu_mini_c::pgmTextPower[] =				"UU.UUUV #1:UI^"
														"UU.UUUV UU.UUV"
														"-I.IIIV  I.IIA"
														". . . . .    .";
													// switch, flash, wrench, check, cancel - ramp/PID
													// in fix mode:
													//	"UU.UUUV #1:FIX"
													//	"UU.UUUV UU.UUV"
													//	"-I.IIIV  I.IIA"
													//	". . . . .    .";
													// in aug mode, unregulated
													//	"UU.UUUV #5:AUG"
													//	"UU.UUUV UU.UUV"
													//	"-I.IIIV  I.IIA"
													//	". . . . .    .";
													// in aug mode, regulated
													//	"UU.UUUV #5:UI^"
													//	"UU.UUUV UU.UUV"
													//	"-I.IIIV  I.IIA"
													//	". . . . .    .";


													//   . . .        .
													//	 01234567890123
const char menu_mini_c::pgmTextProfile[] =				"# 1 / n  (...)"
														"U=UU.UU-UU.UUV"
														"I= I.IIA max  "
														". . .        .";
													//	ramp, check, cancel - falsh

													//   . . . .      .
													//   .=auto  .=100%
													//	 01234567890123
const char menu_mini_c::pgmTextSettings[] =				"\x81=auto  \x86=100%"	// ramp / sun
														"\x80=auto        "		// switch
														"\x85=auto        "		// pid
														". . . .      .";
													// ramp, mark, check, cancel - wrench

													//   . . . .      .
													//	 01234567890123
const char menu_mini_c::pgmTextCalibration[] =			"I-Calibration "
														"int.: I=I.IIIA"
														"ref.: I=I.IIIA"
														". . . .      .";
													//	ramp, trash, check. cancel - mark


													//	 01234567890123
const char menu_mini_c::pgmTextRemote[] =				"UU.UUUV #1:UI "
														"UU.UUUV UU.UUV"
														"-I.IIIV  I.IIA"
														" REMOTE      .";
														 



const tft_mini_c::icon_et menu_mini_c::pgmIconsPower[]  =
														{	tft_mini_c::ICON_RAMP,
															tft_mini_c::ICON_SWITCH,
															tft_mini_c::ICON_FLASH,
															tft_mini_c::ICON_WRENCH,
															tft_mini_c::ICON_CHECK,
															tft_mini_c::ICON_CANCEL};

 const tft_mini_c::icon_et menu_mini_c::pgmIconsProfile[]  =
														{	tft_mini_c::ICON_FLASH,
															tft_mini_c::ICON_RAMP,
															tft_mini_c::ICON_CHECK,
															tft_mini_c::ICON_CANCEL};

 const tft_mini_c::icon_et menu_mini_c::pgmIconsSettings[]  =
														{ 	tft_mini_c::ICON_WRENCH,
															tft_mini_c::ICON_RAMP,
															tft_mini_c::ICON_MARK,
															tft_mini_c::ICON_CHECK,
															tft_mini_c::ICON_CANCEL};

 const tft_mini_c::icon_et menu_mini_c::pgmIconsCalibration[]  =
														{ 	tft_mini_c::ICON_MARK,
															tft_mini_c::ICON_RAMP,
															tft_mini_c::ICON_TRASH,
															tft_mini_c::ICON_CHECK,
															tft_mini_c::ICON_CANCEL};

 const menu_mini_c::menu_pos_st menu_mini_c::pgmMenuPower[]  =
	{	{	menu_item_switch,	menu_item_switch, 	menu_mode_select,  		   0, 3,    0},
		{	menu_item_profile,	menu_item_profile,	menu_mode_select,  		   2, 3,    0},
		{	menu_item_settings,	menu_item_settings,	menu_mode_select,  		   4, 3,    0},
		{	menu_item_activate1,menu_item_activate1,menu_mode_select_edit_pos,13, 1,    0},
		{	menu_item_activate2,menu_item_activate2,menu_mode_select_edit_pos,13, 2,    0},
		{	menu_item_edit,		menu_item_activate1,menu_mode_edit,			   9, 1, 1000},
		{	menu_item_edit,		menu_item_activate1,menu_mode_edit,			  12, 1,   20},
		{	menu_item_edit,		menu_item_activate2,menu_mode_edit,			   9, 2, 1000},
		{	menu_item_edit,		menu_item_activate2,menu_mode_edit,			  11, 2,  100},
		{	menu_item_edit,		menu_item_activate2,menu_mode_edit,			  12, 2,   10},
		{	menu_item_ok,		menu_item_end, 		menu_mode_select_edit,	   6, 3,    0},
		{	menu_item_cancel,   menu_item_end,		menu_mode_select_edit,	   8, 3,    0},
		{	menu_item_end,   	menu_item_end,		menu_mode_off,			   0, 0,    0}};

const menu_mini_c::menu_pos_st menu_mini_c::pgmMenuProfile[]  =
	{	{	menu_item_power,	menu_item_power, 	menu_mode_select,	   	   0, 3, 0},
		{	menu_item_radio1,	menu_item_radio1,	menu_mode_select_edit_pos, 0, 0, 0},
		{	menu_item_ok,		menu_item_end, 		menu_mode_select_edit,     2, 3, 0},
		{	menu_item_cancel,	menu_item_end,		menu_mode_select_edit,	   4, 3, 0},
		{	menu_item_end,  	menu_item_end,		menu_mode_off,			   0, 0, 0}};

const menu_mini_c::menu_pos_st menu_mini_c::pgmMenuSettings[]  =
	{	{	menu_item_power,		menu_item_power, 		menu_mode_select,	   	   0, 3, 0},
		{	menu_item_calibration,	menu_item_calibration,	menu_mode_select,		   2, 3, 0},
		{	menu_item_radio1,		menu_item_radio1,		menu_mode_select_edit_pos, 0, 0, 0},
		{	menu_item_radio2,		menu_item_radio2,		menu_mode_select_edit_pos, 0, 1, 0},
		{	menu_item_radio3,		menu_item_radio3,		menu_mode_select_edit_pos, 0, 2, 0},
		{	menu_item_radio4,		menu_item_radio4,		menu_mode_select_edit_pos, 8, 0, 5},
		{	menu_item_ok,			menu_item_end, 			menu_mode_select_edit,	   4, 3, 0},
		{	menu_item_cancel,		menu_item_end,			menu_mode_select_edit,	   6, 3, 0},
		{	menu_item_end,  		menu_item_end,			menu_mode_off,		   	   0, 0, 0}};

const menu_mini_c::menu_pos_st menu_mini_c::pgmMenuCalibration[]  =
	{	{	menu_item_power,		menu_item_power, 	menu_mode_select,		   0, 3,     0},
		{	menu_item_delete,		menu_item_delete,	menu_mode_select,	   	   2, 3,     0},
		{	menu_item_activate1,	menu_item_activate1,menu_mode_select_edit_pos,13, 2,     0},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			   8, 2,  1000},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  10, 2,   100},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  11, 2,    10},
		{	menu_item_edit,			menu_item_activate1,menu_mode_edit,			  12, 2,     1},
		{	menu_item_ok,			menu_item_end, 		menu_mode_select_edit,	   4, 3,     0},
		{	menu_item_cancel,		menu_item_end,		menu_mode_select_edit,	   6, 3,     0},
		{	menu_item_end,  		menu_item_end,		menu_mode_off,			   0, 0,     0}};

const menu_mini_c::menu_pos_st menu_mini_c::pgmMenuDelete[]  =
	{	{	menu_item_ok,			menu_item_end, 		menu_mode_select_edit,	   4, 3,     0},
		{	menu_item_cancel,		menu_item_end,		menu_mode_select_edit,	   6, 3,     0},
		{	menu_item_end,  		menu_item_end,		menu_mode_off,	   	   0, 0,     0}};


menu_mini_c::menu_mini_c(controller_c *  controller, parameter * Parameter)
{	// menu
	main_state=menu_init_start;
	this->controller=controller;
	this->Parameter=Parameter;
}	// menu

void menu_mini_c::process(void)
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

			button_ok.init();
			button_up.init();
			button_dn.init();


			MENU_DEBUG_PRINTLN2(F("LCD"));

			Lcd.init(20,4,tft_mini_c::spread);
			Lcd.backlightOn(true);
			Lcd.putChar(0,0,pgmTextStart);
			Lcd.putChar(5,2,Version.pgmGetVersion());
			Lcd.putChar(1,3,Version.pgmGetBuiltDate());

			main_state=menu_init_delay;
			temp=menuTime;
			break;
		case menu_init_delay:
			if ((uint16_t)(menuTime-temp)>INIT_DELAY)
			{
				MENU_DEBUG_PRINTLN2("Reading Config");
				Lcd.Clear();
				Lcd.putChar(0,2,"read config  ");
				Lcd.progressBarInit(0,3,14,0);
				main_state=menu_read_parameter;
			}
			break;
		case menu_read_parameter:
			if (Parameter->process())
			{	// parameter busy
				MENU_DEBUG_PRINT2(Parameter->getProgress());
				MENU_DEBUG_PRINTLN2("%");
				Lcd.progressSet(Parameter->getProgress());
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

				Lcd.progressSet(0);
				Lcd.putChar(0,2,"Source Caps  ");
				main_state=menu_get_pd_data;
				MENU_DEBUG_PRINTLN2("reading Source Caps");
			}	// parameter done
			break;
		case menu_get_pd_data:
			if (controller->is_up())
			{	// PD Caps available
				MENU_DEBUG_PRINTLN2("menu_get_pd_data");
				Lcd.progressBarDisable();
				main_state=menu_set_parameter;

				/*
				 * moved to UI.process()
				setOptions();
				 */
			}	// PD Caps available
			break;
		case menu_set_parameter:
			if (controller->is_power_ready())
			{
				MENU_DEBUG_PRINTLN2("menu_set_parameter");
				active_menu=menu_item_power;
				main_state=menu_set_parameter;
				Lcd.putChar(0,2,"configure PPS");
				main_state=menu_start;
				/*
				 * moved to  UI.process
				controller->enable_output(Parameter->getAutoOn());
				 */
				lastUpdate=menuTime;
			}
			break;
		case menu_start:
			main_state=menu_process;
			menu_mode=menu_mode_off;
			lock_item=menu_item_end;
			menu_item=0;
			lastAction=0;
			Lcd.setBrightness(Parameter->getBrightness());
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
				case menu_item_remote:
					draw_menu(pgmTextRemote,NULL);
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
				case menu_item_remote:
					doRemote();
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
				if (active_menu!=menu_item_remote)
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
}	// process

void menu_mini_c::forceUpdate(void)
{   // forceUpdate
    // set update Flag
    force_update=true;
    main_state=menu_wait_pd_transition;
}   // forceUpdate


void menu_mini_c::setRemote(bool bRemote)
{	// setRemote
	if (bRemote)
	{	//	activate Remote Menu
		active_menu=menu_item_remote;
	}	//	activate Remote Menu
	else
	{	// 	activate Power Menu
		active_menu=menu_item_power;
	}	// 	activate Power Menu
	main_state=menu_start;
	menu_mode=menu_mode_off;
	set_cursor(0,0);
}	// setRemote


void menu_mini_c::draw_menu(const char * menuText, const tft_mini_c::icon_et * iconList)
{	// draw menu
	Lcd.Clear();
	Lcd.setColour(tft_mini_c::WHITE);
	Lcd.setFontFace(tft_mini_c::font_regular);
	Lcd.putChar(0,0,menuText);
	if (iconList)
	{
		uint8_t n=0;
		Lcd.putChar(13,3,tft_mini_c::ICON_OFFSET+(uint8_t)iconList[n]);
		do
		{	// set icons
			n++;
			Lcd.putChar((n-1)*2,3,tft_mini_c::ICON_OFFSET+(uint8_t)iconList[n]);
		}	// set icons
		while (iconList[n]!=tft_mini_c::ICON_CANCEL);
	}
}	// draw menu

// process MenuTable,
void menu_mini_c::process_button(const menu_pos_st * menuPos)
{
	buttonState.buttonPressed=button_ok.pressed();
	buttonState.buttonDown=button_ok.down();
	buttonState.buttonHeld=button_ok.long_press();
	if (button_dn.pressed())
		buttonState.buttonTurns=-1;
	else if (button_up.pressed())
		buttonState.buttonTurns=1;
	else
		buttonState.buttonTurns=0;

	if (buttonState.buttonPressed)
	{	// button pressed
		if (menuPos[menu_item].menu_item==menu_item_cancel)
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
				if (menuPos[menu_item].menu_item<menu_item_menu_end)
				{	// switch between menus
					main_state=menu_start;
					menu_mode=menu_mode_off;
					active_menu=menuPos[menu_item].menu_item;
				}	// switch between menus
				else
				{	// switch to select edit mode
					menu_mode=menuPos[menu_item].menu_action;
					lock_item=menuPos[menu_item].lock_item;
				}	// switch to select edit mode
			}	// button pressed
			break;
		case menu_mode_select_edit:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				menu_mode=menuPos[menu_item].menu_action;
				lock_item=menuPos[menu_item].lock_item;
			}	// button pressed
			break;
		case menu_mode_select_edit_pos:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				menu_mode=menuPos[menu_item].menu_action;
				if (menu_mode==menu_mode_select_edit_pos)
					menu_mode=menu_mode_select_edit;
			}	// button pressed
			break;
		case menu_mode_edit:
			if (buttonState.buttonPressed)
			{ 	// button pressed
				menu_mode=menuPos[menu_item].menu_action;
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

	set_cursor(menuPos[menu_item].x_pos,menuPos[menu_item].y_pos);
}

void menu_mini_c::next_menu_item(const menu_pos_st * menuPos)
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

			bOk=bOk && (menuPos[menu_item].menu_item!=menu_item_end);

			bOk=bOk && ( ( (menu_mode==menu_mode_select_edit_pos) &&
						   (menuPos[menu_item].lock_item==lock_item) ) ||
						 ( (menu_mode==menu_mode_select_edit) &&
						   (menuPos[menu_item].menu_action>menu_mode_select) &&
						   (menuPos[menu_item].menu_action!=menu_mode_edit) ) ||
						 ( (menu_mode==menu_mode_select) &&
						   (menuPos[menu_item].menu_action!=menu_mode_edit) &&
						   (menuPos[menu_item].lock_item!=menu_item_end) )  );

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


void menu_mini_c::set_cursor(uint8_t x, uint8_t y)
{	// set_curosr

	switch (menu_mode)
	{	// switch menu_mode
		case menu_mode_off:
			/*
			Lcd.CursorOn(false);
			Lcd.CursorBlink(false);
			*/
			Lcd.CursorMode(tft_mini_c::cursor_off);
			break;
		case menu_mode_select:
			/*
			Lcd.CursorBlink(true);
			*/
			Lcd.CursorMode(tft_mini_c::cursor_block);
			break;
		case menu_mode_select_edit:
			/*
			Lcd.CursorOn(true);
			Lcd.CursorBlink(false);
			*/
			Lcd.CursorMode(tft_mini_c::cursor_line);
			break;
		case menu_mode_select_edit_pos:
			/*
			Lcd.CursorOn(true);
			Lcd.CursorBlink(true);
			*/
			Lcd.CursorMode(tft_mini_c::cursor_frame);
			break;
		case menu_mode_edit:
			/*
			Lcd.CursorOn(true);
			Lcd.CursorBlink(false);
			*/
			Lcd.CursorMode(tft_mini_c::cursor_tray);
			break;
	}	// switch menu_mode
	Lcd.CursorPos(x,y);
}	// set_curosr

uint8_t menu_mini_c::menu_pos_items(const menu_pos_st * menuPos)
{	// menu_pos_items
	uint8_t n=0;

	while (menuPos[n].menu_item!=menu_item_end) n++;
	return n;
} 	// menu_pos_items


void menu_mini_c::doPower(void)
{	// do Power Menu
	uint16_t lastUpdateDelta=menuTime-lastUpdate;

	if (((lastUpdateDelta>(UPDATE_CYCLE)) && (menu_mode==menu_mode_off)) ||
		((lastUpdateDelta>(UPDATE_CYCLE_SET)) && (menu_mode!=menu_mode_off)) ||
		force_update)
	{	// update voltage and current readings
		// Update voltage & current settings and readings
		lastUpdate=menuTime;

		// display settings independent from Menu
		if (force_update)
		{	// force_update
			PD_power_info_t power_info;
			bool			bOk;
			bOk=controller->get_profile_info(controller->get_current_profile(),&power_info);

			Lcd.putChar( 9,0,'1'+controller->get_current_profile());
			Lcd.putChar(11,0,controller->get_profile_text(power_info.type));				

			if ((controller->get_operating_mode()!=controller_c::CONTROLLER_MODE_OFF) &&
				(controller->is_PPS()))
			{
				Lcd.putChar(13,3,tft_mini_c::ICON_OFFSET+(char)tft_mini_c::ICON_PID);
				switch (controller->get_operating_mode())
				{
					case controller_c::CONTROLLER_MODE_CV:
						Lcd.putChar(11,0,"U  ");
						break;
					case controller_c::CONTROLLER_MODE_CVCC:
						Lcd.putChar(11,0,"UI ");
						break;
					case controller_c::CONTROLLER_MODE_CVCCmax:
						Lcd.putChar(11,0,"UI^");
						break;
					default:
						break;
				}
			}
			else
			{
				Lcd.putChar(13,3,tft_mini_c::ICON_OFFSET+(char)tft_mini_c::ICON_RAMP);
			}
			force_update=false;
		}	// force_update

		Lcd.setColour(tft_mini_c::WHITE);
		Lcd.setFontFace(tft_mini_c::font_regular);
		Lcd.putChar(0,0,convert_c(controller->get_Vbus_mV(),3).getStringUnsigned(lineBuffer,6,3,"V"));

		if (controller->is_output_enabled())
		{	// output is on
			if (controller->is_PPS())
			{	// pps yellow, cyan or red
				if ((controller->get_operating_mode() !=
					 controller_c::CONTROLLER_MODE_OFF))
				{	// mode is pps refulated
					if (controller->is_constant_current_active())
					{	// constand current mode, show readings red
						Lcd.setColour(tft_mini_c::RED);
					}	// constand current mode, show readings red
					else
					{	// no overload, show cyan
						Lcd.setColour(tft_mini_c::CYAN);
					}	// no overload, show cyan
				}	// mode is pps refulated
				else
				{	// pps unregulated, yellow
					Lcd.setColour(tft_mini_c::YELLOW);
				}	// pps unregulated, yellow
			}	// pps yellow, cyan or red
			else
			{	//  FIX, green
					Lcd.setColour(tft_mini_c::GREEN);
			}	//  FIX, green

		}	// output is on
		else
		{	// output is off
			// show readings in grey
			Lcd.setColour(tft_mini_c::DARK_GREY);
		}	// output is off
		Lcd.setFontFace(tft_mini_c::font_bold);
		Lcd.putChar(0,1,convert_c(controller->get_output_voltage_mV(),3).getStringUnsigned(lineBuffer,6,3,"V"));
		Lcd.putChar(0,2,convert_c(controller->get_output_current_mA(),3).getStringSigned(lineBuffer,6,3,"A"));
		
		Lcd.setColour(tft_mini_c::WHITE);
		if (Parameter->hasSD())
			Lcd.putChar(11,3,tft_mini_c::ICON_OFFSET+(uint8_t)tft_mini_c::ICON_SDCARD);
		else
			Lcd.putChar(11,3,' ');
	}	// update voltage and current readings

	Lcd.setColour(tft_mini_c::WHITE);
	Lcd.setFontFace(tft_mini_c::font_regular);
	Lcd.putChar(8,1,convert_c(power_edit.uEdit,3).getStringUnsigned(lineBuffer,5,2,"V"));
	Lcd.putChar(9,2,convert_c(power_edit.iEdit,3).getStringUnsigned(lineBuffer,4,2,"A"));

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
							power_edit.uEdit+=pgmMenuPower[menu_item].edit_val;
						else
							power_edit.uEdit-=pgmMenuPower[menu_item].edit_val;
						if (power_edit.uEdit>controller->get_max_voltage_mV(power_edit.iEdit))
							power_edit.uEdit=controller->get_max_voltage_mV(power_edit.iEdit);
						if (power_edit.uEdit<controller->get_min_voltage_mV())
							power_edit.uEdit=controller->get_min_voltage_mV();
						break;
					case menu_item_activate2:
						if (buttonState.buttonTurns>0)
							power_edit.iEdit+=pgmMenuPower[menu_item].edit_val;
						else
							power_edit.iEdit-=pgmMenuPower[menu_item].edit_val;
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

void menu_mini_c::doProfile(void)
{	// Profile Menu
	if (force_update)
	{	// force_update
		controller->get_profile_info(profile_edit.currentProfile,&profile_edit.PowerInfo);
		Lcd.putChar(2,0,convert_c(profile_edit.currentProfile+1,0).getStringUnsigned(lineBuffer,1,0));
		Lcd.putChar(6,0,convert_c(controller->get_profile_cnt(),0).getStringUnsigned(lineBuffer,1,0));
		Lcd.putChar(10,0,controller->get_profile_text(profile_edit.PowerInfo.type));

		Lcd.putChar(2,2,convert_c(profile_edit.PowerInfo.max_i,3).getStringUnsigned(lineBuffer,5,2,"A"));

		if (profile_edit.PowerInfo.type==PD_PDO_TYPE_FIXED_SUPPLY)
		{	// fix, show min only
			Lcd.putChar(2,1,convert_c(profile_edit.PowerInfo.max_v,3).getStringUnsigned(lineBuffer,5,2,"V      "));
		}	// fix, show min only
		else
		{	// pps, show min / max
			Lcd.putChar(2,1,convert_c(profile_edit.PowerInfo.min_v,3).getStringUnsigned(lineBuffer,5,2,"-"));
			Lcd.putChar(8,1,convert_c(profile_edit.PowerInfo.max_v,3).getStringUnsigned(lineBuffer,5,2,"V"));
			if (profile_edit.PowerInfo.type==PD_PDO_TYPE_BATTERY)
			{	// Battery profile, show max power
                Lcd.putChar(2,2,convert_c(profile_edit.PowerInfo.max_p,2).getStringUnsigned(lineBuffer,5,2,"W"));
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

void menu_mini_c::doSettings(void)
{	// do Settings Menu

	if (force_update)
	{	// force_update
		if (settings_edit.AutoSet)
			Lcd.putChar(2,0,"auto");
		else
			Lcd.putChar(2,0,"man ");

		if (settings_edit.AutoOn)
			Lcd.putChar(2,1,"auto");
		else
			Lcd.putChar(2,1,"man ");

		switch ((controller_c::operating_mode_et)settings_edit.CVCC)
		{
			case controller_c::CONTROLLER_MODE_CV:
				Lcd.putChar(2,2,"CV   ");
				break;
			case controller_c::CONTROLLER_MODE_CVCC:
				Lcd.putChar(2,2,"CVCC ");
				break;
			case controller_c::CONTROLLER_MODE_CVCCmax:
				Lcd.putChar(2,2,"CVCC^");
				break;
			case controller_c::CONTROLLER_MODE_OFF:
			/* fall through */
			default:
				settings_edit.CVCC=(uint8_t)controller_c::CONTROLLER_MODE_OFF;
				Lcd.putChar(2,2,"off  ");
				break;
		}

		// Brightness
		Lcd.putChar(10,0,convert_c(settings_edit.Brightness,0).getStringUnsigned(lineBuffer,3,0,"%"));
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
				if (buttonState.buttonTurns>0)
				{  // inkrement brightness
					if (settings_edit.Brightness<100-pgmMenuSettings[menu_item].edit_val)
						settings_edit.Brightness+=pgmMenuSettings[menu_item].edit_val;
					else
						settings_edit.Brightness=100;
				}	// inkrement brightness
				if (buttonState.buttonTurns<0)
				{	// decrement brightness
					if (settings_edit.Brightness>5+pgmMenuSettings[menu_item].edit_val)
						settings_edit.Brightness-=pgmMenuSettings[menu_item].edit_val;
					else
						settings_edit.Brightness=5;
				}	// decrement brightness
				Lcd.setBrightness(settings_edit.Brightness);
				force_update=true;
			}	// turns, edit
			break;
		default:
			break;
	}	// switch menu_item

}	// do Settings Menu

void menu_mini_c::doCalibration(void)
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
		Lcd.putChar(8,1,convert_c(calibration_edit.editCurrent.internal_value, 3).getStringUnsigned(lineBuffer,5,3,"A"));
		Lcd.putChar(8,2,convert_c(calibration_edit.editCurrent.reference_value,3).getStringUnsigned(lineBuffer,5,3,"A"));
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
				Lcd.putChar(13,3,tft_mini_c::ICON_OFFSET+(char)tft_mini_c::ICON_TRASH);
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
                                pgmMenuCalibration[menu_item].edit_val)
								calibration_edit.editCurrent.reference_value +=
									pgmMenuCalibration[menu_item].edit_val;
							else
								calibration_edit.editCurrent.reference_value=CAL_LIMIT_I;
						else
							if (calibration_edit.editCurrent.reference_value >
								pgmMenuCalibration[menu_item].edit_val)
								calibration_edit.editCurrent.reference_value -=
									pgmMenuCalibration[menu_item].edit_val;
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

void menu_mini_c::doDelete(void)
{	// do Delete Menu
	process_button(pgmMenuDelete);

	switch ((menu_item_et)pgm_read_byte(&pgmMenuDelete[menu_item].menu_item))
	{	// switch menu_item
		case menu_item_ok:
			if (buttonState.buttonPressed)
			{	// button pressed
				Parameter->clear(true);
				Lcd.setColour(tft_mini_c::WHITE);
				Lcd.progressBarInit(0,3,14,0);
				main_state=menu_read_parameter;
				menu_mode=menu_mode_off;
				set_cursor(0,0);
			}	// button pressed
			break;
		default:
			break;
	}	// switch menu_item
}	// do Delete Menu

void menu_mini_c::doRemote(void)
{ // doRemote
	uint16_t lastUpdateDelta=menuTime-lastUpdate;

	if (((lastUpdateDelta>(UPDATE_CYCLE)) && (menu_mode==menu_mode_off)) ||
		((lastUpdateDelta>(UPDATE_CYCLE_SET)) && (menu_mode!=menu_mode_off)) ||
		force_update)
	{	// update voltage and current readings
		// Update voltage & current settings and readings
		lastUpdate=menuTime;

		// display settings independent from Menu
		if (force_update)
		{	// force_update
			PD_power_info_t power_info;
			controller->get_profile_info(controller->get_current_profile(),&power_info);

			Lcd.putChar( 9,0,'1'+controller->get_current_profile());
			Lcd.putChar(11,0,controller->get_profile_text(power_info.type));

			if ((controller->get_operating_mode()!=controller_c::CONTROLLER_MODE_OFF) &&
				(controller->is_PPS()))
			{
				Lcd.putChar(13,3,tft_mini_c::ICON_OFFSET+(char)tft_mini_c::ICON_PID);
				switch (controller->get_operating_mode())
				{
					case controller_c::CONTROLLER_MODE_CV:
						Lcd.putChar(11,0,"U  ");
						break;
					case controller_c::CONTROLLER_MODE_CVCC:
						Lcd.putChar(11,0,"UI ");
						break;
					case controller_c::CONTROLLER_MODE_CVCCmax:
						Lcd.putChar(11,0,"UI^");
						break;
					default:
						break;
				}
			}
			else
			{
				Lcd.putChar(13,3,tft_mini_c::ICON_OFFSET+(char)tft_mini_c::ICON_RAMP);
			}
			force_update=false;
		}	// force_update

		Lcd.setColour(tft_mini_c::WHITE);
		Lcd.setFontFace(tft_mini_c::font_regular);
		Lcd.putChar(0,0,convert_c(controller->get_Vbus_mV(),3).getStringUnsigned(lineBuffer,6,3,"V"));

		if (controller->is_output_enabled())
		{	// output is on
			if (controller->is_PPS())
			{	// pps yellow, cyan or red
				if ((controller->get_operating_mode() !=
					 controller_c::CONTROLLER_MODE_OFF))
				{	// mode is pps gefulated
					if (controller->is_constant_current_active())
					{	// constand current mode, show readings red
						Lcd.setColour(tft_mini_c::RED);
					}	// constand current mode, show readings red
					else
					{	// no overload, show cyan
						Lcd.setColour(tft_mini_c::CYAN);
					}	// no overload, show cyan
				}	// mode is pps refulated
				else
				{	// pps unregulated, yellow
					Lcd.setColour(tft_mini_c::YELLOW);
				}	// pps unregulated, yellow
			}	// pps yellow, cyan or red
			else
			{	//  FIX, green
					Lcd.setColour(tft_mini_c::GREEN);
			}	//  FIX, green

		}	// output is on
		else
		{	// output is off
			// show readings in grey
			Lcd.setColour(tft_mini_c::DARK_GREY);
		}	// output is off



		Lcd.setFontFace(tft_mini_c::font_bold);
		Lcd.putChar(0,1,convert_c(controller->get_output_voltage_mV(),3).getStringUnsigned(lineBuffer,6,3,"V"));
		Lcd.putChar(0,2,convert_c(controller->get_output_current_mA(),3).getStringSigned(lineBuffer,6,3,"A"));

		Lcd.setColour(tft_mini_c::WHITE);
		Lcd.setFontFace(tft_mini_c::font_regular);
		Lcd.putChar(8,1,convert_c(controller->get_set_voltage(),3).getStringUnsigned(lineBuffer,5,2,"V"));
		Lcd.putChar(9,2,convert_c(controller->get_set_current(),3).getStringUnsigned(lineBuffer,4,2,"A"));

		if (Parameter->hasSD())
			Lcd.putChar(11,3,tft_mini_c::ICON_OFFSET+(uint8_t)tft_mini_c::ICON_SDCARD);
		else
			Lcd.putChar(11,3,' ');

	}	// update voltage and current readings

} // doRemote

void menu_mini_c::setOptions(void)
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

	Lcd.setBrightness(Parameter->getBrightness());
}	// setOptions
