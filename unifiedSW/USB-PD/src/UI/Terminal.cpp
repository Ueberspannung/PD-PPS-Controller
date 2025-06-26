#include "Terminal.h"
#include "../ASCII/ASCII_ctrl.h"
#include "../ASCII/ASCII_box.h"
#include "../../Version.h"
#include "../tool/convert.h"
#include "ctype.h"

#include "../../config.h"
const char Terminal_c::VoltageDisplayUnit[]=" V";
const char Terminal_c::CurrentDisplayUnit[]=" A";

const char Terminal_c::OutputMenuData[]=	"Output Menu\0"
											"  0.00 (V) \0"
											"  0.00 (A) \0"
											"   xxxxx   \0"
											"In  0.000 V\0\0";

const char Terminal_c::OutputOff[]= 		"   (O)ff   ";
const char Terminal_c::OutputOn[]= 			"   (O)n    ";

const char Terminal_c::RegulatorMenuData[]=	"(R)egulator\0"
											" OFF       \0"
											" CV        \0"
											" CV CC     \0"
											" CV CC max \0";
										//	"123456789012345678901234"
const char Terminal_c::ProfileMenuDataS[]=	"(P)rofile               \0\0"
											" FIX     5.00 V / 3.00 A\0"
											" 3.30 - 21.00 V / 5.00 A\0"
											"                        \0"
											"                        \0"
											"                        \0"
											"                        \0"
											"                        \0";

										//	"123456789012345678901234"
const char Terminal_c::AutoStartMenuData[]=	"     Auto (S)tartup     \0"
											"  Off  \0"
											"  Set  \0"
											"  On  \0\0";

										//	"12345678901234567890123456"
const char Terminal_c::CalibrationMenuData[]=
											" Current (C)alibration    \0"
											"Int: 0.000 A\0"
											"Ref: 0.000 A\0\0";
										//	"12345678901234567890123456"
const char Terminal_c::EraseDataMenuData[]= " (E)rase EEPROM Data: N   ";

										//	"123456789012345678901234"
const char Terminal_c::TitleData[]=			" PD - PPS \0"
											"Controller\0\0";

										//	"123456789012345678901234"
const char Terminal_c::VersionData[]=		"Version   \0\0";
const char Terminal_c::BuiltData[]=			"Built     \0\0";

										//	"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
const char Terminal_c::WaitMenu[]=			"Keys: (A) (C) (E) (O) (P) (R) (S) (V)";
const char Terminal_c::VerticalMenu[]=  	"Keys: (CRSR_UP) (CRSR_DOWN) (ESC) (ENTER)";
const char Terminal_c::HorizontalMenu[]=	"Keys: (CRSR_LEFT) (CRSR_RIGHT)   (ESC)   (ENTER)";
const char Terminal_c::EditMenu[]=			"Keys: (CRSR_LEFT) (CRSR_RIGHT)  (BS) (DEL)  (ESC) (Enter)  (0) - (9) (.) (SPACE)";
const char Terminal_c::YesNoMenu[]=			"Keys: (DEL)  (ESC) (Enter)  (N) (n) (Y) (y)";

const char Terminal_c::NumEditChars[]=		" .0123456789";
const char Terminal_c::YesNoChars[]=		"NnYy";
const char Terminal_c::MainMenuChars[]=		"AaCcEeOoPpRrSsVv";




void Terminal_c::init(VT100_c *term, controller_c * Controller, parameter * Parameter)
{
	pTerminal	=term;
	pController	=Controller;
	pParameter	=Parameter;
}


void Terminal_c::begin(void)
{
    MenuMode=START;
    outputVoltageEdit=0;
    outputCurrentEdit=0;
    CalibrationCurrentInternal=0;
    CalibrationCurrentEdit=0;
    bOutputSwitch=false;
    bAutoSet=false;
    bAutoOn=false;
    ProfileSelection=1;
    RegulatorSelection=1;
}


bool Terminal_c::process(void)
{	// process
    bool tempUpdate;
	TimeStamp=pController->clock_ms();
	switch (MenuMode)
	{	// switch MenuMode
		case START:
			pTerminal->ResetTerminal();
			pTerminal->SetCursorOn(false);
			pTerminal->SetColours(default_text_colour,	default_background_colour);
			pTerminal->ClearScreen();

			resetPower();

			printTitle();
			printVersion();
			MenuMode=PARAMETER;
			lastUpdate=0;
			break;
		case PARAMETER:
			if (pParameter->process())
			{
				if (lastUpdate!=pParameter->getProgress())
				{
					lastUpdate=pParameter->getProgress();
					pTerminal->SaveRestoreCursorAttributes(true);
					pTerminal->SetColours(VT100_c::white,VT100_c::red);
					pTerminal->PrintAt(1,24,"processing parameter ");
					pTerminal->print(lastUpdate);
					pTerminal->print("%");
					pTerminal->SaveRestoreCursorAttributes(false);
				}
			}
			else
			{
				MenuMode=CONTROLLER;
				pTerminal->SetCursor(1,24);
				pTerminal->ClearLine();
				pTerminal->SaveRestoreCursorAttributes(true);
				pTerminal->SetColours(VT100_c::white,VT100_c::red);
				pTerminal->PrintAt(1,24,"waiting for controller");
				pTerminal->SaveRestoreCursorAttributes(false);
			}
			break;
		case CONTROLLER:
			if (pController->is_up())
			{
				MenuMode=POWER;
				pTerminal->SetCursor(1,24);
				pTerminal->ClearLine();
				pTerminal->SaveRestoreCursorAttributes(true);
				pTerminal->SetColours(VT100_c::white,VT100_c::red);
				pTerminal->PrintAt(1,24,"waiting for power");
				pTerminal->SaveRestoreCursorAttributes(false);
			}
			break;
		case POWER:
			if (pController->is_power_ready())
			{
				ShowMenu();
				MenuMode=WAITING;
				MenuItem=IDLE;
				DrawStatusLineText(WaitMenu);
				createPower();
				updatePower(pController->get_current_profile());
				forceUpdate=true;
			}
			break;
		case WAITING:
			updateData();
			processWaiting(MenuMode,MenuItem);
			break;
		case EDIT:
			if (ProcessEdit())
				EndEdit(MenuMode,MenuItem);
			break;
		case LIST:
            if (ProcessListMenu())
                EndListMenu(MenuMode,MenuItem);

			break;
		default:
			MenuMode=START;
			break;
	}	// switch MenuMode


	tempUpdate=externalUpdate && (MenuMode==WAITING);
	externalUpdate=externalUpdate && !tempUpdate;
	return tempUpdate;
}	// process

void Terminal_c::end(void)
{
}



void Terminal_c::ShowMenu(void)
{
	pTerminal->ResetTerminal();
	pTerminal->SetCursorOn(false);
	pTerminal->SetColours(default_text_colour,	default_background_colour);
	pTerminal->ClearScreen();

	DrawBox(VoltageDisplayXpos,
			VoltageDisplayYpos,
			VoltageDisplayXsize,
			VoltageDisplayYsize,
			VoltageDisplayDouble);
	DrawBox(CurrentDisplayXpos,
			CurrentDisplayYpos,
			CurrentDisplayXsize,
			CurrentDisplayYsize,
			CurrentDisplayDouble);

	DrawVerticalFrame(	OutputMenuXpos,
                        OutputMenuYpos,
                        OutputMenuSize,
                        OutputMenuItems,
                        OutputMenuLast);

	DrawVerticalFrame(	RegulatorMenuXpos,
                        RegulatorMenuYpos,
                        RegulatorMenuSize,
                        RegulatorMenuItems,
                        RegulatorMenuLast);

	DrawVerticalFrame(	ProfileMenuXpos,
                        ProfileMenuYpos,
                        ProfileMenuSize,
                        ProfileMenuItems,
                        ProfileMenuLast);

	DrawHorizontalFrame(AutoStartMenuXpos,
						AutoStartMenuYpos,
						AutoStartMenuSize,
						AutoStartMenuItems);


	DrawHorizontalFrame(CalibrationMenuXpos,
                        CalibrationMenuYpos,
                        CalibrationMenuSize,
                        CalibrationMenuItems);

	DrawBox(EraseDataMenuXpos,
			EraseDataMenuYpos,
			EraseDataMenuXSize,
			EraseDataMenuYSize,
			EraseDataMenuDouble);


	// Textoutput

	DrawVerticalText(	OutputMenuXpos,
                        OutputMenuYpos,
                        OutputMenuData);


	DrawVerticalText(	ProfileMenuXpos,
                        ProfileMenuYpos,
                        ProfileMenuData,
                        ProfileSelection,0,
                        ProfileMenuLast);

	printTitle();
	printVersion();

	DrawHorizontalText(	CalibrationMenuXpos,
						CalibrationMenuYpos,
						CalibrationMenuSize,
						CalibrationMenuData);

	pTerminal->SaveRestoreCursorAttributes(true);
	pTerminal->SetAttributes(VT100_c::reset|VT100_c::dim);
	pTerminal->PrintAt(EraseDataMenuXpos+1,EraseDataEditYpos,EraseDataMenuData);
	pTerminal->SaveRestoreCursorAttributes(false);

}


void Terminal_c::ClearBlock(uint8_t Xpos, uint8_t Ypos, uint8_t width, uint8_t lines)
{	// ClearBlock
	while (lines-->0)
	{
		pTerminal->SetCursor(Xpos,Ypos+lines);
		for (uint8_t n=0; n<width; n++) pTerminal->write(SPACE);
	}
}	// ClearBlock

void Terminal_c::DrawBox(uint8_t Xpos, uint8_t Ypos, uint8_t Xsize, uint8_t Ysize, bool doubleSize)
{
	uint8_t i,j;

	i=0;
	j=0;

	if (doubleSize)
	{
		for (j=Ypos;j<Ypos+(Ysize<<1);j+=2)
		{
			i++;
			pTerminal->SetCursor(Xpos,j);
			pTerminal->SetCharSize(VT100_c::double_hight_upper_line);
			if (i==1) DrawUpperLine(Xsize);
			else if (i==Ysize) DrawLowerLine(Xsize);
			else DrawFieldLine(Xsize);
			pTerminal->SetCursor(Xpos,j+1);
			pTerminal->SetCharSize(VT100_c::double_hight_lower_line);
			if (i==1) DrawUpperLine(Xsize);
			else if (i==Ysize) DrawLowerLine(Xsize);
			else DrawFieldLine(Xsize);
		}
	}
	else
	{
		pTerminal->SetCursor(Xpos,Ypos);
		DrawUpperLine(Xsize);
		Ysize--;
		for (j=1;j<Ysize;j++)
		{
			pTerminal->SetCursor(Xpos,Ypos+j);
			DrawFieldLine(Xsize);
		}
		pTerminal->SetCursor(Xpos,Ypos+Ysize);
		DrawLowerLine(Xsize);
	}
}

void Terminal_c::DrawVerticalFrame(	uint8_t Xpos, uint8_t Ypos,	// Position of the Menu Frame
									uint8_t Size, 				// X-width of the Frame
									uint8_t Items, 				// numebr of entries (excluding Header)
									uint8_t Last)				// hight of last entry
{
	pTerminal->SetCursor(Xpos,Ypos++); DrawUpperLine(Size);
	pTerminal->SetCursor(Xpos,Ypos++); DrawFieldLine(Size);
	pTerminal->SetCursor(Xpos,Ypos++); DrawIntermediateLine(Size,true);
	while (Items-->1)
	{
		pTerminal->SetCursor(Xpos,Ypos++); DrawFieldLine(Size);
		pTerminal->SetCursor(Xpos,Ypos++); DrawIntermediateLine(Size,false);
	}
	while (Last--)
	{
		pTerminal->SetCursor(Xpos,Ypos++); DrawFieldLine(Size);
	}
	pTerminal->SetCursor(Xpos,Ypos++); DrawLowerLine(Size);
}

void Terminal_c::DrawHorizontalFrame(uint8_t Xpos, uint8_t Ypos, uint8_t Size, uint8_t Items)
{	// DrawHorizontalFrame
	DrawVerticalFrame(Xpos,Ypos,Size,1,1);
    Ypos+=2;
	while(Items>1)
    {
        Xpos+=(Size/Items);
        Size-=Size/Items;
        Items--;
        pTerminal->PutCharAt(Xpos,Ypos,BOX_HDTS);
        pTerminal->PutCharAt(Xpos,Ypos+1,BOX_VS);
        pTerminal->PutCharAt(Xpos,Ypos+2,BOX_HDBS);
    }
}	// DrawHorizontalFrame


void Terminal_c::DrawUpperLine(uint8_t Length)
{
	pTerminal->write(BOX_TLD);
	Length-=2;
	while (Length--) pTerminal->write(BOX_HD);
	pTerminal->write(BOX_TRD);
}

void Terminal_c::DrawFieldLine(uint8_t Length)
{
	pTerminal->write(BOX_VD);
	Length-=2;
	while (Length--) pTerminal->write(SPACE);
	pTerminal->write(BOX_VD);
}

void Terminal_c::DrawIntermediateLine(uint8_t Length, bool doubleLine)
{
	pTerminal->write((doubleLine)?(BOX_VLD):(BOX_VDLS));
	Length-=2;
	while (Length--) pTerminal->write((doubleLine)?(BOX_HD):(BOX_HS));
	pTerminal->write((doubleLine)?(BOX_VRD):(BOX_VDRS));
}

void Terminal_c::DrawLowerLine(uint8_t Length)
{
	pTerminal->write(BOX_BLD);
	Length-=2;
	while (Length--) pTerminal->write(BOX_HD);
	pTerminal->write(BOX_BRD);
}

void Terminal_c::SetTextAttribute(	bool Selected,
									bool Highlighted)
{
	pTerminal->SetAttributes(VT100_c::reset|VT100_c::dim);
	pTerminal->SetColours(default_text_colour,	default_background_colour);
	if (Selected)
		pTerminal->SetAttributes(VT100_c::bright);
	if (Highlighted)
		pTerminal->SetAttributes(VT100_c::reverse);
}


void Terminal_c::DrawVerticalText(	uint8_t Xpos, uint8_t Ypos,	// Position of List Menu
								const char * Data,			// pointer to List Menu data
								uint8_t Selected,			// number of slected item 0=none
								uint8_t Highlighted,		// number of higlghtes item 0=none
								uint8_t Last,				// hight of last item
								uint8_t ItemCnt)			// mumber of items in Data list exlcuding Header;
{
	uint8_t i=0;
	if (ItemCnt==0) ItemCnt=CountMenuData(Data)-Last;
	DrawVerticalText(	Xpos,Ypos,
						i,
						Data,
						false,
						Highlighted!=0);
	for (i=1; i<=ItemCnt; i++)
	{
		Data=NextMenuItem(Data);
		DrawVerticalText(	Xpos,Ypos,
						i,
						Data,
						i==Selected,
						i==Highlighted,
						(i==ItemCnt)&&(Last>1));
	}
}

void Terminal_c::DrawVerticalText(	uint8_t Xpos, uint8_t Ypos,	// Position of List Menu
									uint8_t Item,				// Position of Item
									const char * Data,			// pointer to List Menu data
									bool Selected,				// slected item
									bool Highlighted,			// higlghted item
									bool last)					// last entry multiple lines
{	// DrawVerticalText
	Xpos+=1;
	Ypos+=1+(Item<<1);
	pTerminal->SaveRestoreCursorAttributes(true);

	SetTextAttribute(Selected,Highlighted);
	do
	{
		pTerminal->PrintAt(Xpos,Ypos,Data);
		Ypos+=1;
		Data=NextMenuItem(Data);
	}
	while (*Data && last);
	pTerminal->SaveRestoreCursorAttributes(false);
}	// DrawVerticalText


void Terminal_c::DrawHorizontalText(uint8_t Xpos, uint8_t Ypos,	// Position of decision Menu
									uint8_t Size, 				// Width of deceision Menu
									const char * Data, 			// pointer to decision Menu Data
									uint8_t Selected, 			// number of selected item
									uint8_t Highlighted)     	// mumber of highlighted item
{	//	DrawHorizontalText
	uint8_t ItemCnt=CountMenuData(Data)-1;
	int i=0;
	DrawHorizontalText(	Xpos,Ypos,
						Size,ItemCnt,
						i,
						Data,
						false,
						Highlighted!=0);
	for (i=1; i<=ItemCnt; i++)
	{
		Data=NextMenuItem(Data);
		DrawHorizontalText(	Xpos,Ypos,
							Size,ItemCnt,
							i,
							Data,
							i==Selected,
							i==Highlighted);
	}

}	//	DrawHorizontalText

void Terminal_c::DrawHorizontalText(uint8_t Xpos, uint8_t Ypos,	// Position of List Menu
									uint8_t Size, uint8_t Items,// Size and numeber of Items
									uint8_t Item,				// Count and Position of Item
									const char * Data,			// pointer to List Menu data
									bool Selected,				// slected item
									bool Highlighted)					// last entry multiple lines
{
	Xpos++;
	Ypos++;
	if (Item>0)
	{
		Ypos+=2;
		Item--;
	}

	while (Item>0)
	{
        Xpos+=(Size/Items);
        Size-=Size/Items;
        Items--;
        Item--;
	}
	pTerminal->SaveRestoreCursorAttributes(true);
	SetTextAttribute(Selected,Highlighted);
	pTerminal->PrintAt(Xpos,Ypos,Data);
	pTerminal->SaveRestoreCursorAttributes(false);

}

void Terminal_c::DrawBoxText(	uint8_t Xpos, uint8_t Ypos,
								const char* Data,
								bool doubleSize,
								uint8_t Ysize)
{	// DrawBoxText
	Xpos++;
	Ypos+=(doubleSize)?(2):(1);
	Ysize-=2;
	while ((Ysize--) && (*Data))
	{
		pTerminal->PrintAt(Xpos,Ypos++,Data);
		if (doubleSize)
			pTerminal->PrintAt(Xpos,Ypos++,Data);
		Data=NextMenuItem(Data);
	}
}	// DrawBoxText


void Terminal_c::DrawStatusLineText(const char *Data)
{	// DrawStatusLineText
	pTerminal->SetCursor(StatusXpos,StatusYpos);
	pTerminal->ClearLine();
	pTerminal->print(Data);
}	// DrawStatusLineText


uint8_t Terminal_c::CountMenuData(const char * Data)
{	// CountMenuData
	uint8_t n=0;
	while (*Data)
	{	// Valid String
		n++;
		while (*Data++); 	// skip until next
	}	// Valid String
	return n;
}	// CountMenuData

const char * Terminal_c::NextMenuItem(const char * Data)
{	// NextMenuItem
	while (*Data++); 	// skip until next
	return Data;
}	// NextMenuItem

void Terminal_c::printTitle(void)
{	// printTitle
	pTerminal->SetCursor(1,1); pTerminal->SetCharSize(VT100_c::double_hight_upper_line);
	pTerminal->SetCursor(1,2); pTerminal->SetCharSize(VT100_c::double_hight_lower_line);
	pTerminal->SetCursor(1,3); pTerminal->SetCharSize(VT100_c::double_hight_upper_line);
	pTerminal->SetCursor(1,4); pTerminal->SetCharSize(VT100_c::double_hight_lower_line);

	pTerminal->PrintAt(TitleXpos,TitleYpos,TitleData);
	pTerminal->PrintAt(TitleXpos,TitleYpos+1,TitleData);
	pTerminal->PrintAt(TitleXpos,TitleYpos+2,NextMenuItem(TitleData));
	pTerminal->PrintAt(TitleXpos,TitleYpos+3,NextMenuItem(TitleData));


}	// printTitle

void Terminal_c::printVersion(void)
{	// printVersion
	DrawVerticalFrame(	VersionXpos,
						VersionYpos,
						VersionSize,
						VersionItems);

	DrawVerticalFrame(	BuiltXpos,
						BuiltYpos,
						BuiltSize,
						BuiltItems);

	DrawVerticalText(	VersionXpos,VersionYpos,VersionData);
	DrawVerticalText(	BuiltXpos,BuiltYpos,BuiltData);
	DrawVerticalText(	VersionXpos,VersionYpos,1,Version_c().pgmGetVersion());
	DrawVerticalText(	BuiltXpos,BuiltYpos,1,Version_c().pgmGetBuiltDate());
}	// printVersion


void Terminal_c::updateData(void)
{	// updateData
	if (forceUpdate)
	{	// unconditional of settings
		uint16_t Cal,Ref;
		pParameter->getCalI(&Cal,&Ref);
		updateCalibration(	Cal,Ref);
		updateSetVoltage(	pController->get_set_voltage());
		updateSetCurrent(	pController->get_set_current());
		updateAutoStart(	pParameter->getAutoSet(),
							pParameter->getAutoOn());
	}	// unconditional of settings

	if (forceUpdate || ((uint16_t)(TimeStamp-lastUpdate)>UPDATE_CYCLE))
	{	// update of readings
		lastUpdate=TimeStamp;
		updateOutputReadings(	pController->get_output_voltage_mV(),
								pController->get_output_current_mA(),
								pController->is_output_enabled(),
								pController->is_PPS(),
								pController->is_constant_current_active(),
								(pController->get_operating_mode()!=controller_c::CONTROLLER_MODE_OFF) &&
								pController->is_PPS());
		updateInVoltage( 		pController->get_Vbus_mV());
		updateOutputSwitch(		pController->is_output_enabled());
		updateRegulator(		pController->get_operating_mode());

		if (pController->is_PPS())
			pTerminal->PrintAt(53,20,"PPS");
		else if (pController->is_PD())
			pTerminal->PrintAt(53,20,"FIX");
		else
			pTerminal->PrintAt(53,20,"N/A");

		if (pController->is_ps_transition())
			pTerminal->PrintAt(57,20,"in trans  ");
		else
			pTerminal->PrintAt(57,20,"trans done");

		if (pController->is_power_ready())
			pTerminal->PrintAt(68,20,"ready");
		else
			pTerminal->PrintAt(68,20,"busy ");
	}	// update of readings
	forceUpdate=false;
}	// updateData

void Terminal_c::updateOutputReadings(	uint16_t Voltage_mV,
										int16_t Current_mA,
										bool on, bool pps, bool cc, bool regulated)
{	// updateOutputReadings
	pTerminal->SaveRestoreCursorAttributes(true);
	pTerminal->SetAttributes(VT100_c::reset);

	if (on)
	{	// on: different colours
		pTerminal->SetAttributes(VT100_c::bright);
		if (!pps)
			pTerminal->SetColours(fixed_colour,default_background_colour);
		else if (cc)
			pTerminal->SetColours(current_limiting_colour,default_background_colour);
		else if (regulated)
			pTerminal->SetColours(regulated_colour,default_background_colour);
		else
			pTerminal->SetColours(unregulated_colour,default_background_colour);
	}	// on: different colours
	else
	{	// just readings, no colour
		pTerminal->SetAttributes(VT100_c::dim);
		pTerminal->SetColours(default_text_colour,default_background_colour);
	}	// just readings, no colour
	DrawBoxText(VoltageDisplayXpos,
				VoltageDisplayYpos,
				convert_c(	Voltage_mV,
							millisDecimals).getStringUnsigned(	buffer,
																VoltageDisplaySize,
																VoltageDisplayDecimals,
																VoltageDisplayUnit),
				VoltageDisplayDouble,
				VoltageDisplayYsize);
	DrawBoxText(CurrentDisplayXpos,
				CurrentDisplayYpos,
				convert_c(	Current_mA,
							millisDecimals).getStringSigned(	buffer,
																CurrentDisplaySize,
																CurrentDisplayDecimals,
																CurrentDisplayUnit),
				CurrentDisplayDouble,
				CurrentDisplayYsize);

	pTerminal->SaveRestoreCursorAttributes(false);
}	// updateOutputReadings

void Terminal_c::updateSetVoltage(uint16_t Voltage_mV)
{	// updateSetVoltage
	pTerminal->PrintAt(	VoltageEditXpos,VoltageEditYpos,
						convert_c(	Voltage_mV,
									millisDecimals).getStringUnsigned(	buffer,
																		VoltageEditLen,
																		OutputDecimals));
}	// updateSetVoltage

void Terminal_c::updateSetCurrent(uint16_t Current_mA)
{	// updateSetCurrent
	pTerminal->PrintAt(	CurrentEditXpos,CurrentEditYpos,
						convert_c(	Current_mA,
									millisDecimals).getStringUnsigned(	buffer,
																		CurrentEditLen,
																		OutputDecimals));
}	// updateSetCurrent

void Terminal_c::updateInVoltage(uint16_t Voltage_mV)
{	// updateInVoltage
	if (forceUpdate ||
		(VbusCycleCnt>Vbus_cycle_max) ||
		(VbusVoltage+Vbus_limit_max<Voltage_mV) ||
		(VbusVoltage>Vbus_limit_max+Voltage_mV) )
	{	// Update
		VbusVoltage=Voltage_mV;
		VbusCycleCnt=0;
		pTerminal->PrintAt(	InputVoltageXpos,InputVoltageYpos,
							convert_c(	VbusVoltage,
										millisDecimals).getStringUnsigned(	buffer,
																			InputVoltageSize,
																			InputVoltageDecimals));
	}	// Update
	else if ((VbusVoltage+Vbus_limit_min<Voltage_mV) ||
			 (VbusVoltage>Vbus_limit_min+Voltage_mV) )
		VbusCycleCnt++;
	else
		VbusCycleCnt=0;
}	// updateInVoltage

void Terminal_c::updateOutputSwitch(bool bOn)
{	//	updateOutputSwitch
	pTerminal->PrintAt(	OutputStateXpos,
						OutputStateYpos,
						(bOn)?(OutputOn):(OutputOff));
}	// updateOutputSwitch

void Terminal_c::updateCalibration(uint16_t Internal_mA, uint16_t Reference_mA)
{	// updateCalibration
	pTerminal->PrintAt(	CalibrationIntXpos,CalibrationIntYpos,
						convert_c(	Internal_mA,
									millisDecimals).getStringUnsigned(	buffer,
																		CalibrationEditLen,
																		CalibrationDecimals));
	pTerminal->PrintAt(	CalibrationEditXpos,CalibrationEditYpos,
						convert_c(	Reference_mA,
									millisDecimals).getStringUnsigned(	buffer,
																		CalibrationEditLen,
																		CalibrationDecimals));
}	// updateCalibration

void Terminal_c::updateAutoStart(bool bAutoSet, bool bAutoOn)
{	// updateAutoStart
	DrawHorizontalText(	AutoStartMenuXpos,
						AutoStartMenuYpos,
						AutoStartMenuSize,
						AutoStartMenuData,
						getAutoStartupVal(bAutoSet,bAutoOn));
}	// updateAutoStart

void Terminal_c::updateRegulator(controller_c::operating_mode_et mode)
{	// updaterRegulator
	DrawVerticalText(	RegulatorMenuXpos,
						RegulatorMenuYpos,
						RegulatorMenuData,
						(uint8_t)mode+1);

}	// updaterRegulator

void Terminal_c::updatePower(uint8_t profile)
{	// updatePower
	DrawVerticalText(	ProfileMenuXpos,
                        ProfileMenuYpos,
                        ProfileMenuData,
                        profile+1,0,
                        ProfileMenuLast);
}	// updatePower

void Terminal_c::createPower(void)
{	// createPower
	PD_power_info_t PowerInfo;

	resetPower();

	if (pController->get_profile_cnt()>0)
	{	// Profiles to process
		uint8_t n=0;
		uint8_t LineStart=0;
		while (pController->get_profile_info(n++,&PowerInfo))
		{	// scan available profiles
			if (!ProfileMenuLast) ProfileMenuItems++;

			LineStart=(ProfileMenuItemLen+1)*n;
			for(uint8_t i=0; i<ProfileMenuItemLen; i++)
				ProfileMenuData[LineStart+i]=SPACE;
			ProfileMenuData[LineStart+ProfileMenuItemLen]=NUL;

			memcpy(	&ProfileMenuData[LineStart+ProfileMenuOffsetText],
					pController->get_profile_text(PowerInfo.type),3);

			ProfileMenuData[LineStart+ProfileMenuOffsetSlash]='/';

			switch (PowerInfo.type)
			{	// Type
				case PD_PDO_TYPE_FIXED_SUPPLY:
					memcpy(	&ProfileMenuData[LineStart+ProfileMenuOffsetUmax],
							convert_c(	PowerInfo.max_v,
										millisDecimals).getStringUnsigned(	buffer,
																			ProfileMenuValSizeV,
																			ProfileMenuValDecimals,
																			" V"),
							ProfileMenuValSizeV+2);
					memcpy(	&ProfileMenuData[LineStart+ProfileMenuOffsetImax],
							convert_c(	PowerInfo.max_i,
										millisDecimals).getStringUnsigned(	buffer,
																			ProfileMenuValSizeA,
																			ProfileMenuValDecimals,
																			" A"),
							ProfileMenuValSizeA+2);
					break;
				case PD_PDO_TYPE_BATTERY:
					break;
				case PD_PDO_TYPE_VARIABLE_SUPPLY:
					break;
				case PD_PDO_TYPE_AUGMENTED_PDO:
					ProfileMenuData[LineStart+ProfileMenuOffsetDash]='-';
					memcpy(	&ProfileMenuData[LineStart+ProfileMenuOffsetUmin],
							convert_c(	PowerInfo.min_v,
										millisDecimals).getStringUnsigned(	buffer,
																			ProfileMenuValSizeV,
																			ProfileMenuValDecimals),
							ProfileMenuValSizeV);
					memcpy(	&ProfileMenuData[LineStart+ProfileMenuOffsetUmax],
							convert_c(	PowerInfo.max_v,
										millisDecimals).getStringUnsigned(	buffer,
																			ProfileMenuValSizeV,
																			ProfileMenuValDecimals,
																			" V"),
							ProfileMenuValSizeV+2);
					memcpy(	&ProfileMenuData[LineStart+ProfileMenuOffsetImax],
							convert_c(	PowerInfo.max_i,
										millisDecimals).getStringUnsigned(	buffer,
																			ProfileMenuValSizeA,
																			ProfileMenuValDecimals,
																			" A"),
							ProfileMenuValSizeA+2);
					ProfileMenuLast++;
					break;
				default:
					break;
			}	// Type
		}	// scan available profiles
		ProfileMenuData[(ProfileMenuItemLen+1)*n]=NUL;
		if ((ProfileMenuItems>0) && (ProfileMenuLast==0))
			ProfileMenuLast=1;
	}	// Profiles to process
	DrawVerticalFrame(	ProfileMenuXpos,
						ProfileMenuYpos,
						ProfileMenuSize,
						ProfileMenuItems,
						ProfileMenuLast);
}	// createPower

void Terminal_c::resetPower(void)
{	// createPower
	ClearBlock(ProfileMenuXpos,ProfileMenuYpos,ProfileMenuSize,ProfileMenuLines);
	ProfileMenuItems=ProfileMenuItemsS;
	ProfileMenuLast	=ProfileMenuLastS;
	memcpy(ProfileMenuData,ProfileMenuDataS,ProfileMenuDataSize);
}	// createPower

void Terminal_c::StartListMenu(uint8_t Xpos, uint8_t Ypos, const char * Data, uint8_t Size, uint8_t Last, uint8_t Selection, bool Horizontal)
{   // StartListMenu
    listMenuData.Xpos=Xpos;
    listMenuData.Ypos=Ypos;
    listMenuData.Data=Data;
    listMenuData.Size=Size;
    listMenuData.Items=CountMenuData(Data)-Last;
    listMenuData.Last=Last;
    listMenuData.Selection=Selection;
    listMenuData.Highlighted=Selection;
    listMenuData.Horizontal=Horizontal;
    listMenuData.updated=false;
    if (Horizontal)
    {	// Horizontal
        DrawHorizontalText( listMenuData.Xpos, listMenuData.Ypos,
						    listMenuData.Size,
					        listMenuData.Data,
						    listMenuData.Selection,
						    listMenuData.Highlighted);
		DrawStatusLineText(HorizontalMenu);
	}	// Horizontal
    else
    {	// Vertical
        DrawVerticalText(   listMenuData.Xpos, listMenuData.Ypos,
							listMenuData.Data,
							listMenuData.Selection,
							listMenuData.Highlighted,
                            listMenuData.Last);
		DrawStatusLineText(VerticalMenu);
    }	// Vertical
}   // StartListMenu

bool Terminal_c::ProcessListMenu(void)
{
    char cKey=0;
    bool bKey;
    if (listMenuData.Horizontal)
        bKey=pTerminal->ScanKeyMask(&cKey,"",HorizontalControl);
    else
        bKey=pTerminal->ScanKeyMask(&cKey,"",VerticalControl);
    if (bKey)
    {
        switch (cKey)
        {   // switch key
            case VT100_c::EDIT_CRSR_UP:
                /* FALL THROUGH */
            case VT100_c::EDIT_CRSR_LEFT:
                if (listMenuData.Highlighted>1)
                    listMenuData.Highlighted--;
                break;
            case VT100_c::EDIT_CRSR_DOWN:
                /* FALL THROUGH */
            case VT100_c::EDIT_CRSR_RIGHT:
                if (listMenuData.Highlighted<listMenuData.Items)
                    listMenuData.Highlighted++;
                break;
            case VT100_c::EDIT_ENTER:
                listMenuData.Selection=listMenuData.Highlighted;
                /* FALL THROUGH */
            case VT100_c::EDIT_ESC:
                listMenuData.updated=cKey==VT100_c::EDIT_ENTER;
                listMenuData.Highlighted=0;
                break;
            default:
                break;
        }   // switch key
        if (listMenuData.Horizontal)
            DrawHorizontalText( listMenuData.Xpos,
                                listMenuData.Ypos,
                                listMenuData.Size,
                                listMenuData.Data,
                                listMenuData.Selection,
                                listMenuData.Highlighted);
        else
            DrawVerticalText(   listMenuData.Xpos,
                                listMenuData.Ypos,
                                listMenuData.Data,
                                listMenuData.Selection,
                                listMenuData.Highlighted,
                                listMenuData.Last);
    }
    return bKey && ((cKey==VT100_c::EDIT_ESC) || (cKey==VT100_c::EDIT_ENTER));
}

void Terminal_c::EndListMenu(menuMode_et &mode,	menuItem_et &item)
{   // EndListMenu
	mode=WAITING;
	if (listMenuData.updated)
		switch (item)
		{   // switch Item
			case STARTUP:
				// update Startup Settings
				SetAutoStart(listMenuData.Selection);
				break;
			case PROFILE:
				// update Profile Settings
				mode=SetProfile(listMenuData.Selection);
				break;
			case REGULATOR:
				// update Regulator Settings
				SetRegulator(listMenuData.Selection);
				break;
			default:
				break;
		}   // switch Item
    item=IDLE;
	DrawStatusLineText(WaitMenu);
	externalUpdate=true;
}   // EndListMenu

void Terminal_c::StartEditNum(uint8_t Xpos, uint8_t Ypos, uint16_t val, uint8_t Size, uint8_t Decimals)
{	// StartEdit
    listMenuData.Xpos=Xpos;
    listMenuData.Ypos=Ypos;
    listMenuData.Size=Size;
    listMenuData.Items=Decimals;
	DrawStatusLineText(EditMenu);
	pTerminal->EditStart(	Xpos,Ypos,
							convert_c(	val,
										millisDecimals).getStringUnsigned(	buffer,
																			Size,
																			Decimals),
							Size,
							NumEditChars);
}	// StartEdit

void Terminal_c::StartEditYesNo(uint8_t Xpos, uint8_t Ypos, char val)
{	// StartEdit
    listMenuData.Xpos=Xpos;
    listMenuData.Ypos=Ypos;
    listMenuData.Size=1;
    buffer[0]=val;
    buffer[1]=NUL;
	DrawStatusLineText(YesNoMenu);
	pTerminal->EditStart(Xpos,Ypos,buffer,1,YesNoChars);
}	// StartEdit

bool Terminal_c::ProcessEdit(void)
{	// ProcessEdit
	bool bEnd;
	switch (pTerminal->Edit())
	{
		case VT100_c::cancel:
			bEnd=true;
			listMenuData.updated=false;
			break;
		case VT100_c::enter:
			bEnd=true;
			listMenuData.updated=true;
			break;
		default:
			bEnd=false;
			break;
	}
	return bEnd;
}	// ProcessEdit

void Terminal_c::EndEdit(menuMode_et &mode,	menuItem_et &item)
{	// EndEdit
	mode=WAITING;
    if (listMenuData.updated)
    {   // modified, new value

		if (item!=ERASE)
		{	// numerical input
			uint16_t temp=convert_c(buffer).getUnsigned(millisDecimals);

			switch (item)
			{
				case VOLTAGE:
					temp=SetOutputVoltage(temp);
					outputVoltageEdit=temp;
					break;
				case CURRENT:
					temp=SetOutputCurrent(temp);
					outputCurrentEdit=temp;
					break;
				case CALIBRATION:
					SetCalibration(temp);
					CalibrationCurrentEdit=temp;
					break;
				default:
					break;
			}
			convert_c(  temp,
						millisDecimals).getStringUnsigned(  buffer,
															listMenuData.Size,
															listMenuData.Items);
		} // numerical value
		else
		{	// erase Data
			buffer[0]=toupper(buffer[0]);

			if (buffer[0]=='Y')
			{	// ok, it's your choice
				pParameter->clear(true);
				mode=PARAMETER;
			}	// ok, it's your choice
		}	// erase Data
	}   // modified, new value
	pTerminal->PrintAt(listMenuData.Xpos,listMenuData.Ypos,buffer);
	item=IDLE;
	if (mode==WAITING)
		DrawStatusLineText(WaitMenu);
	else
		DrawStatusLineText("");
	externalUpdate=true;
}	// EndEdit


void Terminal_c::processWaiting(menuMode_et &mode,	menuItem_et &item)
{
    char cKey;

    if (pTerminal->ScanKeyMask(&cKey,MainMenuChars))
    { // key pressed
        cKey=toupper(cKey); // make Upper
        switch (cKey)
        {
            case 'A': // edit current setting
				if (pController->is_PPS())
				{	// edit only in PPS mode
					mode=EDIT;
					item=CURRENT;
				}	// edit only in PPS mode
				else
				{	// select different profile
					mode=LIST;
					item=PROFILE;
				}	// select different profile
                break;
			case 'E': // Erase EEPROM data
				mode=EDIT;
				item=ERASE;
				break;
            case 'C': // edit calibration value
				mode=EDIT;
				item=CALIBRATION;
                break;
            case 'O': // Toggel Output
				mode=WAITING;
				item=OUTPUT;
                break;
            case 'P': // List menu Profile
                mode=LIST;
                item=PROFILE;
                break;
            case 'R': // List menu Regulator
                mode=LIST;
                item=REGULATOR;
                break;
            case 'S': // List menu Auto Startup
                mode=LIST;
                item=STARTUP;
                break;
            case 'V': // edit voltage setting
				if (pController->is_PPS())
				{	// edit only in PPS mode
					mode=EDIT;
					item=VOLTAGE;
				}	// edit only in PPS mode
				else
				{	// select different profile
					mode=LIST;
					item=PROFILE;
				}	// select different profile
                break;
            default:
                break;
        }
        StartMenu(item);
    } // key pressed
}


void Terminal_c::StartMenu(menuItem_et &item)
{
	switch (item)
    {
        case CURRENT: // edit current setting
			StartEditNum(	CurrentEditXpos,CurrentEditYpos,
							pController->get_set_current(),
							CurrentEditLen,OutputDecimals);
            break;
        case CALIBRATION: // edit calibration value
			pParameter->getCalI(&CalibrationCurrentInternal,
								&CalibrationCurrentEdit);
			CalibrationCurrentInternal=pController->get_output_current_mA();
			if (CalibrationCurrentInternal&0x8000) CalibrationCurrentInternal=0;
			updateCalibration(CalibrationCurrentInternal,CalibrationCurrentEdit);
			StartEditNum(	CalibrationEditXpos,CalibrationEditYpos,
							CalibrationCurrentEdit,
							CalibrationEditLen,CalibrationDecimals);
            break;
        case OUTPUT: // Toggel Output
			SetOutputSwitch();
            item=IDLE;
            break;
        case PROFILE: // List menu Profile
			ProfileSelection=0;
			if (pController->get_profile_cnt())
			{	// profiles available
				ProfileSelection=pController->get_current_profile()+1;
			}	// profiles available
			StartListMenu(	ProfileMenuXpos, ProfileMenuYpos,
							ProfileMenuData, ProfileMenuSize,
							ProfileMenuLast, ProfileSelection);
            break;
        case REGULATOR: // List menu Regulator
			RegulatorSelection=pController->get_operating_mode()+1;
            StartListMenu(	RegulatorMenuXpos, RegulatorMenuYpos,
							RegulatorMenuData, RegulatorMenuSize,
							RegulatorMenuLast, RegulatorSelection);
            break;
        case STARTUP: // List menu Auto Startup
			bAutoOn=pParameter->getAutoOn();
			bAutoSet=pParameter->getAutoSet();
            StartListMenu(	AutoStartMenuXpos, AutoStartMenuYpos,
							AutoStartMenuData, AutoStartMenuSize,
							AutoStartMenuLast,
							getAutoStartupVal(	bAutoSet,bAutoOn),
							true);
            break;
        case VOLTAGE: // edit voltage setting
			outputVoltageEdit=pController->get_set_voltage();
			StartEditNum(	VoltageEditXpos,VoltageEditYpos,
							outputVoltageEdit,
							VoltageEditLen,OutputDecimals);
            break;
        case ERASE:
			StartEditYesNo(EraseDataEditXpos,EraseDataEditYpos,'N');
			break;
        default:
            break;
	}
}

void Terminal_c::SetOutputSwitch(void)
{	// SetOutputSwitch
	bOutputSwitch=pController->is_output_enabled();
	bOutputSwitch=!bOutputSwitch;
	updateOutputSwitch(bOutputSwitch);
	pController->enable_output(bOutputSwitch);

	if (pParameter->getAutoOn())
	{	// Auto-> save
		pParameter->setAutoOn(bOutputSwitch);
		pParameter->write();
	}	// Auto-> save
}	// SetOutputSwitch

uint16_t Terminal_c::SetOutputVoltage(uint16_t Voltage_mV)
{	// // SetOutputVoltage

	Voltage_mV-=Voltage_mV%20;
	outputCurrentEdit=pController->get_set_current();

	if (Voltage_mV<pController->get_min_voltage_mV())
		Voltage_mV=pController->get_min_voltage_mV();
	if (Voltage_mV>pController->get_max_voltage_mV(outputCurrentEdit))
		Voltage_mV=pController->get_max_voltage_mV(outputCurrentEdit);

	if (outputVoltageEdit!=Voltage_mV)
	{ // new Value, set
		outputVoltageEdit=Voltage_mV;
		pController->set_power(outputVoltageEdit,outputCurrentEdit);
		if (pParameter->getAutoSet())
		{	// Auto -> Save
			pParameter->setVoltage_mV(outputVoltageEdit);
			pParameter->write();
		}	// Auto -> Save
	} // new Value, set
	return outputVoltageEdit;
}	// SetOutputVoltage

uint16_t Terminal_c::SetOutputCurrent(uint16_t Current_mA)
{	// SetOutputCurrent
	Current_mA-=Current_mA%10;
	outputVoltageEdit=pController->get_set_voltage();
	if (Current_mA>pController->get_max_current_mA(outputVoltageEdit))
		Current_mA=pController->get_max_current_mA(outputVoltageEdit);

	if (outputCurrentEdit!=Current_mA)
	{ // new Value, set
		outputCurrentEdit=Current_mA;
		pController->set_power(outputVoltageEdit,outputCurrentEdit);
		if (pParameter->getAutoSet())
		{	// Auto -> Save
			pParameter->setCurrent_mA(outputCurrentEdit);
			pParameter->write();
		}	// Auto -> Save
	} // new Value, set

	return outputCurrentEdit;
}	// SetOutputCurrent


void Terminal_c::SetCalibration(uint16_t Ref_mA)
{	// setCalibration
	CalibrationCurrentEdit=Ref_mA;
	pParameter->setCalI(CalibrationCurrentInternal,
						CalibrationCurrentEdit);
	pParameter->write();
	pController->set_output_current_calibration(CalibrationCurrentInternal,
												CalibrationCurrentEdit);
	forceUpdate=true;
}	// setCalibration

Terminal_c::menuMode_et Terminal_c::SetProfile(uint8_t Profile)
{	// setProfile
	menuMode_et mode=WAITING;
	if ((ProfileSelection!=Profile) && (Profile>0))
	{

		PD_power_info_t PowerInfo;
		ProfileSelection=Profile;
		Profile--;
		outputCurrentEdit=pController->get_set_current();
		outputVoltageEdit=pController->get_set_voltage();
		pController->get_profile_info(Profile,&PowerInfo);


		if (PowerInfo.type!=PD_PDO_TYPE_FIXED_SUPPLY)
		{	// pps, check limits
			if (outputVoltageEdit>PowerInfo.max_v)
				outputVoltageEdit=PowerInfo.max_v;
			if (outputVoltageEdit<PowerInfo.min_v)
				outputVoltageEdit=PowerInfo.min_v;
			if (outputCurrentEdit>pController->get_max_current_mA(outputVoltageEdit))
				outputCurrentEdit=pController->get_max_current_mA(outputVoltageEdit);
			if (outputCurrentEdit<pController->get_min_current_mA())
				outputCurrentEdit=pController->get_min_current_mA();
		}	// pps, check limits
		else
		{	// fixed profile, set
				outputVoltageEdit=PowerInfo.max_v;
				outputCurrentEdit=PowerInfo.max_i;
		}	// fixed profile, set

		if (pParameter->getAutoSet())
		{	// save setting for next start
				pParameter->setVoltage_mV(outputVoltageEdit);
				pParameter->setCurrent_mA(outputCurrentEdit);
				pParameter->setModePPS(	PowerInfo.type !=
										PD_PDO_TYPE_FIXED_SUPPLY);
				pParameter->write();
		}	// save setting for next start


		if (PowerInfo.type == PD_PDO_TYPE_FIXED_SUPPLY)
			pController->set_voltage(outputVoltageEdit);
		else
			pController->set_power(outputVoltageEdit,outputCurrentEdit);

		mode=CONTROLLER;
	}
	return mode;
}	// setProfile

void Terminal_c::SetRegulator(uint8_t Mode)
{	// SetRegulator
	if (Mode!=RegulatorSelection)
	{	// new mode
		RegulatorSelection=Mode;
		pController->set_operating_mode((controller_c::operating_mode_et)(RegulatorSelection-1));
		if (pParameter->getAutoSet())
		{
			pParameter->setCVCC((controller_c::operating_mode_et)(RegulatorSelection-1));
			pParameter->write();
		}
	}	// new mode
}	// SetRegulator

void Terminal_c::SetAutoStart(uint8_t Mode)
{	// SetAutoStart
	if (Mode!=getAutoStartupVal(bAutoOn,bAutoSet))
	{	// new value
		setAutoStartup(Mode,bAutoSet,bAutoOn);
		pParameter->setAutoOn(bAutoOn);
		pParameter->setAutoSet(bAutoSet);
		pParameter->write();
	}	// new value

}	// SetAutoStart

uint8_t Terminal_c::getAutoStartupVal(bool AutoSet, bool AutoOn)
{	// getAutoStartupVal
	return (AutoOn)?(3):((AutoSet)?(2):(1));
}	// getAutoStartupVal

void Terminal_c::setAutoStartup(uint8_t val, bool &AutoSet, bool &AutoOn)
{	// setAutoStartup
	AutoSet=val>=2;
	AutoOn=val==3;
}	// setAutoStartup
