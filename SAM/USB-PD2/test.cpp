#include <Arduino.h>
#include "test.h"


		const test_c::TestSequenz_st test_c::Testsequenz0[]=
													{ 	{  5000, 5000, false, 	5000},
														{  9000, 4000, false, 	5000}, 
														{ 12000, 3000, false, 	5000},
														{ 15000, 2000, false, 	5000},
														{ 20000, 1000, false, 	5000},
														{ 21000,  500, false, 	5000},
														{ 20000, 1000, false, 	5000},
														{ 15000, 2000, false, 	5000},
														{ 12000, 3000, false, 	5000},
														{  9000, 4000, false, 	5000},
														{  5000, 5000, false, 	5000},
														{  3300, 5000, false, 	5000},
														{     0, 5000, false,     0} };

		const test_c::TestSequenz_st test_c::Testsequenz1[]=
													{ 	{  5000, 5000, true, 	5000},
														{  9000, 5000, true,	5000}, 
														{ 12000, 5000, true,	5000},
														{ 15000, 5000, true,	5000},
														{ 20000, 5000, true,	5000},
														{ 15000, 5000, true,	5000},
														{ 12000, 5000, true,	5000},
														{  9000, 5000, true,	5000},
														{     0, 5000, false,    0} };

		const test_c::TestSequenz_st test_c::Testsequenz2[]=
													{ 	{  9000, 5000, true,	5000},
														{  9000, 5000, false,	5000}, 
														{ 12000, 5000, false,	5000},
														{ 12000, 5000, true,	5000},
														{ 15000, 5000, true,	5000},
														{ 15000, 5000, false,	5000},
														{ 20000, 5000, false,	5000},
														{ 20000, 5000, true,	5000},
														{ 15000, 5000, true,	5000},
														{ 15000, 5000, false,	5000},
														{ 12000, 5000, false,	5000},
														{ 12000, 5000, true,	5000},
														{  9000, 5000, true,	5000},
														{  9000, 5000, false,	5000},
														{  5000, 5000, false,	5000},
														{  3300, 5000, false,	5000},
														{  5000, 5000, false,	5000},
														{  5000, 5000, true, 	5000},
														{     0, 5000, false,    0} };

		const test_c::TestSequenz_st test_c::Testsequenz3[]=
													{ 	{  5000, 5000, true,	5000},
														{  5000, 5000, false,	5000}, 
														{     0, 5000, false,    0} };
                                   
		const test_c::TestSequenz_st test_c::Testsequenz4[]=
													{ 	{  5000, 5000, true,	15000},
														{  9000, 5000, true,	15000}, 
														{ 12000, 5000, true,	15000}, 
														{  5000, 3000, false,	15000}, 
														{  5900, 3000, false,	15000}, 
														{  6000, 2000, false,	15000}, 
														{  9000, 2000, false,	15000}, 
														{ 11000, 2000, false,	15000}, 
														{     0, 5000, false,    0} };

		const test_c::TestSequenz_st test_c::Testsequenz5[]=
													{ 	{  9000, 2000, false,	30000}, 
														{     0, 5000, false,    0} };

		const test_c::TestSequenz_st test_c::Testsequenz6[]=
													{ 	{  5000, 5000, true,	20000},
														{  9000, 5000, true,	20000}, 
														{ 12000, 5000, true,	20000},
														{ 15000, 5000, true,	20000},
														{ 20000, 5000, true,	20000},
														{ 15000, 5000, true,	20000},
														{ 12000, 5000, true,	20000},
														{  9000, 5000, true,	20000},
														{  5000, 5000, false,	20000},
														{  9000, 5000, false,	20000}, 
														{ 12000, 5000, false,	20000},
														{ 15000, 5000, false,	20000},
														{ 20000, 5000, false,	20000},
														{ 21000, 5000, false,	20000},
														{ 20000, 5000, false,	20000},
														{ 15000, 5000, false,	20000},
														{ 12000, 5000, false,	20000},
														{  9000, 5000, false,	20000},
														{  5000, 5000, false,	20000},
														{  3300, 5000, false,	20000},
														{     0, 5000, false,    0} };

		const test_c::TestSequenz_st test_c::Testsequenz7[]=
													{ 	
														{ 15000, 2000, false,	  500},
														{ 20000, 2000, false,	 1000},
														{ 21000, 2000, false,	 1500},
														{ 20000, 2000, false,	 1000}, 
														{     0, 5000, false,    0} };

		const test_c::TestSequenz_st * const test_c::Testsequenzen[]= 
															{	Testsequenz0,
																Testsequenz1,
																Testsequenz2,
																Testsequenz3,
																Testsequenz4,
																Testsequenz5,
																Testsequenz6,
																Testsequenz7,
																NULL
																};


void test_c::init(uint8_t testsequenz,controller_c::operating_mode_et operating_mode)
{	// init
	this->testsequenz=testsequenz;
	this->operating_mode=operating_mode;
	testposition=0;
	pinMode(LED_BUILTIN,OUTPUT);
	digitalWrite(LED_BUILTIN,LOW);
	blink=0;
	parameter_timer=0;
	test_timer=0;
	test_state=TEST_STARTUP;
	SerialUSB.begin(115200);
}	// init

void test_c::do_test(void)
{	// do_test
	switch (test_state)
	{	// switch test_state
		case TEST_STARTUP:
			if (!controller.is_up())
			{	
				if ((uint16_t)(millis()-blink)>BLINK_DELAY_START)
				{
					blink=millis();
					digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
				}
			}
			else
				test_state=TEST_WAIT_SERIAL;
			break;
		case TEST_WAIT_SERIAL:
			if (!SerialUSB)
			{	
				if ((uint16_t)(millis()-blink)>BLINK_DELAY_SERIAL)
				{
					blink=millis();
					digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
				}
			}
			else
				test_state=TEST_LIST_PROFILE;
			break;
		case TEST_LIST_PROFILE: 
			digitalWrite(LED_BUILTIN,HIGH);
			print_profiles();
			bStart=true;
			if (controller.get_profile_cnt()>0)
				test_state=TEST_SET_MODE;
			else
				test_state=TEST_END;
			break;
		case TEST_SET_MODE:
			controller.set_operating_mode(operating_mode);
			test_state=TEST_PRINT_PROFILE;
			break;
		case TEST_PRINT_PROFILE: 
			if (controller.is_power_ready())
			{
				print_current_profile();
				test_state=TEST_ENABLE;
			}
			break;
		case TEST_ENABLE:
			if (!controller.is_output_enabled())
			{
				print_time();
				SerialUSB.println(": enable Output");
				test_state=TEST_PRINT_PROFILE;
				controller.enable_output(true);
			}
			else if (bStart)
			{
				bStart=false;
				test_state=TEST_SWITCH;
			}
			else
				test_state=TEST_WAIT;
			break;
		case TEST_WAIT:
			if ((uint16_t)((uint16_t)millis()-parameter_timer)>PARAMETER_UPDATE_TIME)
			{
				parameter_timer=millis();
				print_parameter();
			}
			if ((uint16_t)((uint16_t)millis()-test_timer)>Testsequenzen[testsequenz][testposition].Delay)
			{
				testposition++;
				test_state=TEST_SWITCH;
			}
			break;
		case TEST_SWITCH:
			if (Testsequenzen[testsequenz][testposition].Spannung==0)
				testposition=0;
			
			print_test(testsequenz,testposition);
				
			if (Testsequenzen[testsequenz][testposition].bFix)
				controller.set_voltage(Testsequenzen[testsequenz][testposition].Spannung);
			else
				controller.set_power(
					Testsequenzen[testsequenz][testposition].Spannung,
					Testsequenzen[testsequenz][testposition].Strom);
			
			test_timer=millis();
			
			test_state=TEST_PRINT_PROFILE;
			break;
		case TEST_END:
		/* fall through */
		default:
			break;
	}	// switch test_state

	controller.process();
}	// do_test


/* ********************************************************
 * private functions
 */
void test_c::print_time(void)								// Systemziet in ms als long und uint16_t
{	// print_time
	SerialUSB.print(millis());
	SerialUSB.print(" (");
	SerialUSB.print((uint16_t)millis());
	SerialUSB.print(")");
}	// print_time

void test_c::print_current_profile(void)
{	// print_current_profile
	uint8_t pdo=controller.get_current_profile();
	PD_power_info_t info;
	controller.get_profile_info(pdo,&info);
	print_time();
	SerialUSB.print(": Ubus=");
	SerialUSB.print(controller.get_Vbus_mV());
	SerialUSB.print(" mV => ");
	SerialUSB.print(" #");
	SerialUSB.print(pdo);
	if (controller.get_profile_cnt()==0)
	{	// standard USB Power max 1A
		SerialUSB.print(" ---, ");
	}	// standard USB Power max 1A
	else if (info.type==PD_PDO_TYPE_FIXED_SUPPLY)
	{	// fixed PDO
		SerialUSB.print(" FIX, ");
	}	// fixed PDO
	else
	{	// PPS
			SerialUSB.print(" PPS, ");
	}	// PPS
	SerialUSB.print(" U=");
	SerialUSB.print(controller.get_set_voltage());
	SerialUSB.print(" mV, I=");
	SerialUSB.print(controller.get_set_current());
	SerialUSB.println(" mA");
}	// print_current_profile

void test_c::print_profiles(void)							// liste aller verfügbaren Profile ausgeben
{
	uint8_t lauf=0;
	
	SerialUSB.println();
	SerialUSB.println("***************************************************");

	if (controller.get_profile_cnt()==0) 
	{	// kein PD
		SerialUSB.println("          kein PD/PPS verfügbar");
	}	// kein PD
	else	
	{	// profile listen
		while (lauf<controller.get_profile_cnt())
		{
			PD_power_info_t power_info;
			controller.get_profile_info(lauf,&power_info);
			SerialUSB.print(" #");
			SerialUSB.print(lauf);
			SerialUSB.print(": ");
			switch (power_info.type)
			{
				case PD_PDO_TYPE_FIXED_SUPPLY:
					SerialUSB.print("FIX ");
					break;
				case PD_PDO_TYPE_BATTERY:
					SerialUSB.print("BAT ");
					break;
				case PD_PDO_TYPE_VARIABLE_SUPPLY:
					SerialUSB.print("VAR ");
					break;
				case PD_PDO_TYPE_AUGMENTED_PDO:
					SerialUSB.print("AUG ");
					break;
				default:
					SerialUSB.print("--- ");
					break;
			}
			if (power_info.min_v!=0)
			{
				SerialUSB.print(power_info.min_v);
				SerialUSB.print(" mV ... ");
			}

			SerialUSB.print(power_info.max_v);
			SerialUSB.print(" mV");

			if (power_info.max_i)
			{
				SerialUSB.print(", ");
				SerialUSB.print(power_info.max_i);
				SerialUSB.print(" mA");
			}
			if (power_info.max_p)
			{
				SerialUSB.print(", ");
				SerialUSB.print(power_info.max_p);
				SerialUSB.print("0 mW");
			}
			SerialUSB.println();
			lauf++;
		}
	}	// profile listen
	SerialUSB.println("***************************************************");
}

void test_c::print_parameter(void)
{
	print_time();
	SerialUSB.print(F("Uout="));
	SerialUSB.print(controller.get_output_voltage_mV());
	SerialUSB.print(F(" mV, Iout="));
	SerialUSB.print(controller.get_output_current_mA());
	SerialUSB.print(F(" mA, Ubus="));
	SerialUSB.print(controller.get_Vbus_mV());
	SerialUSB.print(F(" mV, Uset="));
	SerialUSB.print(controller.get_set_voltage());
	SerialUSB.print(F(" mV, Iset="));
	SerialUSB.print(controller.get_set_current());
	SerialUSB.println(F(" mA"));
}

void test_c::print_test(uint8_t sequenz, uint8_t pos)
{	// print_test 
	print_time();
	SerialUSB.print(F(": Test ( "));
	SerialUSB.print(sequenz);
	SerialUSB.print(F(", "));
	SerialUSB.print(pos);
	if (Testsequenzen[sequenz][pos].bFix)
		SerialUSB.print(F(") => Fix U="));
	else
		SerialUSB.print(F(") => PPS U="));
	SerialUSB.print(Testsequenzen[sequenz][pos].Spannung);
	if (Testsequenzen[sequenz][pos].bFix)
	{
		SerialUSB.print(F(" mV, T="));
	}
	else
	{
		SerialUSB.print(F(" mV, I="));
		SerialUSB.print(Testsequenzen[sequenz][pos].Strom);
		SerialUSB.print(F(" mA, T="));
	}
	SerialUSB.print(Testsequenzen[sequenz][pos].Delay);
	SerialUSB.println(F(" ms"));
}	// print_test
