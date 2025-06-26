#ifndef __test_c_h__
#define __test_c_h__

#include <stdint.h>
#include "controller.h"

/* **********************************************************************
 * 
 * TESTSEQUENZ 0:
 * - Initialisierung auf 5 V Fix
 * - dann Wechsel durch PPS Spannungen 
 *   5 - 9 - 12 - 15 - 20 - 21 - 20 - 15 - 12 - 9 - 5 - 3,3
 * 
 *  => 	Beobachtung 
 * 		Wie schon in derrBuglist bei Ryan Ma bemerkt wird 21 V
 * 		zwar gesetzt, aber die Spannugn bleibt bei 20 V
 * 
 * TESTSEQUENZ 1:
 * - Wechsel durch die FIX-Profile 
 *   5 - 9 - 12 - 15 - 20 - 15 -12 -9
 * 
 * TESTSEQUENZ 2:
 * - Abwechsenlnd einen Schirtt im FIX Porfil, 
 *   dann wechsel nach PPS und einen Schritt PPS
 *    5 V Fix,  9 V Fix,  9 V PPS, 12 V PPS, 12 V FIX, 15 V FIX
 *   15 V PPS, 20 V PPS, 20 V FIX, 15 V FIX, 15 V PPS, 12 V PPS
 *   12 V Fix,  9 V Fix,  9 V PPS,  5 V PPS, 3,3 V PPS, 5 V PPS
 * 
 * TESTSEQUENZ 3:
 * - Start im Fix Profil 5V, dann 5V PPS und wieder FIX 
 *    5 V PPS,  5 V Fix
 * 
 * 
 * TESTSEQUENZ 4:
 * - Fix Profile 5 V - 9 V - 12 V 
 * - PPS Profile 5 V / 3 A; 5,9 V / 3 A; 6 V / 2 A; 9 V / 2 A; 11 V / 2 A
 *    5 V PPS,  5 V Fix
 *
 * TESTSEQUENZ 5:
 * - 5 V PPS
 * 
 * TESTSEQUENZ 6:
 * - Fix Profile 5 V - 9 V - 12 V - 15 V - 20 V - 15 V - 12 V - 9 V 
 * - PPS Profil  5 V - 9 V - 12 V - 15 V - 20 V - 21 V - 20 V - 
 * 				15 V -12 V -  9 V -  5 V - 3,3V
 */

class test_c
{
	public:
		void init(uint8_t testsequenz,controller_c::operating_mode_et operating_mode);
		void do_test(void);

	private:
		typedef enum:uint8_t { 	TEST_STARTUP, 
								TEST_WAIT_SERIAL, 
								TEST_LIST_PROFILE, 
								TEST_SET_MODE, 
								TEST_PRINT_PROFILE, 
								TEST_ENABLE, 
								TEST_WAIT,
								TEST_SWITCH,
								TEST_END
								} test_state_et;
	
		typedef struct TestSequenz_s 	{ 	uint16_t Spannung;
											uint16_t Strom;
											bool	 bFix;
											uint16_t Delay;
										}	TestSequenz_st;


		static const TestSequenz_st Testsequenz0[];
		static const TestSequenz_st Testsequenz1[];
		static const TestSequenz_st Testsequenz2[];
		static const TestSequenz_st Testsequenz3[];
		static const TestSequenz_st Testsequenz4[];
		static const TestSequenz_st Testsequenz5[];
		static const TestSequenz_st Testsequenz6[];
		static const TestSequenz_st Testsequenz7[];
		static const TestSequenz_st * const Testsequenzen[];
		
		static const uint16_t BLINK_DELAY_START=100;
		static const uint16_t BLINK_DELAY_SERIAL=1000;
		static const uint16_t PARAMETER_UPDATE_TIME=500;

		void print_time(void);
		void print_current_profile(void);
		void print_profiles(void);							// liste aller verfügbaren Profile ausgeben
		void print_parameter(void);
		void print_test(uint8_t sequenz, uint8_t pos);

		controller_c controller;
		uint16_t blink;
		uint16_t parameter_timer;
		uint16_t test_timer;
		uint8_t testsequenz;
		uint8_t	testposition;
		bool	bStart;
		controller_c::operating_mode_et operating_mode;

		test_state_et test_state;		
};

#endif // __test_c_h__

