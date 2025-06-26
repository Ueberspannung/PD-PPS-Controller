#include <Arduino.h>
#include "controller.h"
#include "messages.h"

#ifdef ARDUINO_ARCH_AVR
	#include <avr/power.h>
#endif

/* *********************************************************************
 * debug macros
 * DEBUG LEVEL 1: regulator steps
 * DEBUG LEVEL 2: controll steps
 */
#define CONTROLLER_DEBUG_LEVEL 0

#if CONTROLLER_DEBUG_LEVEL == 0
	#define CONTROLLER_DEBUG_TIME() 	
	#define CONTROLLER_DEBUG_PRINT(x) 		
	#define CONTROLLER_DEBUG_PRINTLN(x)
	#define CONTROLLER_DEBUG_TIME2() 	
	#define CONTROLLER_DEBUG_PRINT2(x) 		
	#define CONTROLLER_DEBUG_PRINTLN2(x)
	#define CONTROLLER_DEBUG_TIME3() 	
	#define CONTROLLER_DEBUG_PRINT3(x) 		
	#define CONTROLLER_DEBUG_PRINTLN3(x)
#elif CONTROLLER_DEBUG_LEVEL == 1
	#define CONTROLLER_DEBUG_TIME() 		{ Serial1.print(controller_state.process_time); Serial1.print(F(": ")); }
	#define CONTROLLER_DEBUG_PRINT(x) 		Serial1.print(x);
	#define CONTROLLER_DEBUG_PRINTLN(x) 	{ Serial1.print(x); Serial1.print(F("\n")); }
	#define CONTROLLER_DEBUG_TIME2() 	
	#define CONTROLLER_DEBUG_PRINT2(x) 		
	#define CONTROLLER_DEBUG_PRINTLN2(x)
	#define CONTROLLER_DEBUG_TIME3() 	
	#define CONTROLLER_DEBUG_PRINT3(x) 		
	#define CONTROLLER_DEBUG_PRINTLN3(x)
#elif CONTROLLER_DEBUG_LEVEL == 2
	#define CONTROLLER_DEBUG_TIME() 	
	#define CONTROLLER_DEBUG_PRINT(x) 		
	#define CONTROLLER_DEBUG_PRINTLN(x)
	#define CONTROLLER_DEBUG_TIME2() 		{ Serial1.print(controller_state.process_time); Serial1.print(F(": ")); }
	#define CONTROLLER_DEBUG_PRINT2(x) 		Serial1.print(x);
	#define CONTROLLER_DEBUG_PRINTLN2(x) 	{ Serial1.print(x); Serial1.print(F("\n")); }
	#define CONTROLLER_DEBUG_TIME3() 	
	#define CONTROLLER_DEBUG_PRINT3(x) 		
	#define CONTROLLER_DEBUG_PRINTLN3(x)
#elif CONTROLLER_DEBUG_LEVEL == 3
	#define CONTROLLER_DEBUG_TIME() 	
	#define CONTROLLER_DEBUG_PRINT(x) 		
	#define CONTROLLER_DEBUG_PRINTLN(x)
	#define CONTROLLER_DEBUG_TIME2() 		
	#define CONTROLLER_DEBUG_PRINT2(x) 		
	#define CONTROLLER_DEBUG_PRINTLN2(x) 	
	#define CONTROLLER_DEBUG_TIME3() 		{ Serial1.print(controller_state.process_time); Serial1.print(F(": ")); }
	#define CONTROLLER_DEBUG_PRINT3(x) 		Serial1.print(x);
	#define CONTROLLER_DEBUG_PRINTLN3(x)    { Serial1.print(x); Serial1.print(F("\n")); }
#else
#error CONTROLLER_DEBUG_LEVEL ivalid
#endif


const char controller_c::pgmTextPowerProfileFIX[] PROGMEM = "FIX";
const char controller_c::pgmTextPowerProfileBAT[] PROGMEM = "BAT";
const char controller_c::pgmTextPowerProfileVAR[] PROGMEM = "VAR";
const char controller_c::pgmTextPowerProfileAUG[] PROGMEM = "AUG";
const char controller_c::pgmTextPowerProfileNON[] PROGMEM = "---";



void controller_c::printstat(uint16_t Zeile)
{
	static regulator_et lastreg;
	static bool			lastswitch;
	static bool			lastpower;
	static operating_mode_et lastopmode;
	static uint8_t		lastpsstatus;

	if ((lastreg!=RegulatorState.RegulatorStep) ||
		(lastswitch!=controller_state.output_switch) ||
		(lastpower!=controller_state.power_is_ready) ||
		(lastopmode!=controller_state.operating_mode)||
		(lastpsstatus!=PD.get_ps_status()) )
	{
		lastreg=RegulatorState.RegulatorStep;
		lastswitch=controller_state.output_switch;
		lastpower=controller_state.power_is_ready;
		lastopmode=controller_state.operating_mode;
		lastpsstatus=PD.get_ps_status();

		CONTROLLER_DEBUG_TIME3();

		CONTROLLER_DEBUG_PRINT3(": ");
		CONTROLLER_DEBUG_PRINT3(Zeile);
		
		switch (lastreg)
		{
			case REGULATOR_START:
				CONTROLLER_DEBUG_PRINT3(" START  "); 
				break;
			case REGULATOR_MEASURE:
				CONTROLLER_DEBUG_PRINT3(" MEASURE"); 
				break;
			case REGULATOR_WAIT_TRANSITION_START:
				CONTROLLER_DEBUG_PRINT3(" T_START"); 
				break;
			case REGULATOR_WAIT_TRANSITION_END:
				CONTROLLER_DEBUG_PRINT3(" T_END  "); 
				break;
			case REGULATOR_WAIT_DELAY:
				CONTROLLER_DEBUG_PRINT3(" DELAY  "); 
				break;
			case REGULATOR_HOLD:
				CONTROLLER_DEBUG_PRINT3(" HOLD   "); 
				break;
			case REGULATOR_OFF:
				CONTROLLER_DEBUG_PRINT3(" OFF    "); 
				break;
			default:
				CONTROLLER_DEBUG_PRINT3(" DEFAULT"); 
				break;
		}
		
		switch (lastopmode)
		{
			case CONTROLLER_MODE_OFF:
				CONTROLLER_DEBUG_PRINT3(" noReg ")
				break;
			case CONTROLLER_MODE_CV:
				CONTROLLER_DEBUG_PRINT3(" CV    ")
				break;
			case CONTROLLER_MODE_CVCC:
				CONTROLLER_DEBUG_PRINT3(" CVCC  ")
				break;
			case CONTROLLER_MODE_CVCCmax:
				CONTROLLER_DEBUG_PRINT3(" CVCC^ ")
				break;
			default:
				CONTROLLER_DEBUG_PRINT3(" defRg ")
				break;
		}
		
		if (lastswitch)
		{
			CONTROLLER_DEBUG_PRINT3(" ON  ")
		}
		else
		{
			CONTROLLER_DEBUG_PRINT3(" OFF ")
		}
		
		switch (lastpsstatus)
		{
			case STATUS_POWER_NA:
				CONTROLLER_DEBUG_PRINT3(" --- ");
				break;
			case STATUS_POWER_PPS:
				CONTROLLER_DEBUG_PRINT3(" PPS ");
				break;
			case STATUS_POWER_TYP:
				CONTROLLER_DEBUG_PRINT3(" TYP ");
				break;
			default:
				CONTROLLER_DEBUG_PRINT3(" xxx ");
				break;
		}
		
		if (lastpower)
		{
			CONTROLLER_DEBUG_PRINTLN3(" ready")
		}
		else
		{
			CONTROLLER_DEBUG_PRINTLN3(" n.p.")
		}
	}
}

/* *********************************************************************
 * public functions
 */
 
controller_c::controller_c(void)
{
	// todo: basic initialisation
	controller_state.main_state=CONTROLLER_INIT_PD;
	controller_state.process_time=0;
	controller_state.last_process=0;
	controller_state.timer_10ms=0;
	controller_state.timer_100ms=0;
	controller_state.operating_mode=CONTROLLER_MODE_OFF;
	controller_state.set_output_voltage=0;
	controller_state.set_output_current=0;
	controller_state.new_output_voltage=0;
	controller_state.new_output_current=0;
	controller_state.output_voltage=0;
	controller_state.output_current=0;
	controller_state.evaluate_settings=true;
	controller_state.output_switch=false;
	controller_state.monitor_state=MONITOR_SET_BUS_VOLTAGE_RANGE;
	controller_state.pd_transition_state=PD_TRANSITION_START;
	controller_state.power_is_ready=true;
	controller_state.power_transition_timout=false;
	resetRegulator();
}

void controller_c::process(void)
{	// process
	
	controller_state.process_time=PD.clock_ms();

	printstat(__LINE__);
	
	if (controller_state.power_is_ready)
	{	// no transition in Progress, do regulation
		Regulator();					
	}	// no transition in Progress, do regulation
	else
	{	// transition in progress, 
		// no regulation, check transition
		check_PD_transition();
	}	// transition in progress

	printstat(__LINE__);
	
	switch (controller_state.main_state)
	{	// switch main_state
		case CONTROLLER_INIT_PD:
			printstat(__LINE__);
			CONTROLLER_DEBUG_TIME2();
			CONTROLLER_DEBUG_PRINTLN2(F("CONTROLLER_INIT_PD"));
			
			init_output_switch();
			init_power_led();
			init_Vbus_adc();
			init_PD();
			controller_state.main_state=CONTROLLER_INIT_MONITOR;
			
			CONTROLLER_DEBUG_TIME2();
			CONTROLLER_DEBUG_PRINTLN2(F("CONTROLLER_INIT_MONITOR"));
			printstat(__LINE__);
			break;
			
		case CONTROLLER_INIT_MONITOR:
			printstat(__LINE__);
			
			if (init_monitor())
			{
				controller_state.main_state=CONTROLLER_INIT_VBUS_IIR;
			}
			
			printstat(__LINE__);
			break;
			
		case CONTROLLER_INIT_VBUS_IIR:
			printstat(__LINE__);
			
			TODO(erkennen von nicht PD Quellen)
			if (controller_state.power_is_ready)
			{
				CONTROLLER_DEBUG_TIME2();
				CONTROLLER_DEBUG_PRINTLN2(F("PD Ready, CONTROLLER_INIT_VBUS_IIR"));
				Vbus.set(calc_Vbus());
				controller_state.main_state=CONTROLLER_RUN;
			}
			
			printstat(__LINE__);
			break;

		case CONTROLLER_RUN:
			printstat(__LINE__);
			// ms and timer procedures

			if (controller_state.process_time !=
				controller_state.last_process)
			{	// check time dependend operations
			
				controller_state.timer_10ms+= 
					(uint8_t)(	controller_state.process_time -
								controller_state.last_process);
				controller_state.timer_100ms+= 
					(uint8_t)(	controller_state.process_time -
								controller_state.last_process);

				printstat(__LINE__);

				if (controller_state.timer_10ms>=10)
				{	// 10ms timer
					Vbus.add(calc_Vbus());
					controller_state.timer_10ms=0;
				}	// 10ms timer
				
				printstat(__LINE__);
				
				if (controller_state.timer_100ms>=100)
				{	// 100ms timer
					uint16_t temp;
					if (monitor.conversion_ready(&temp))
					{	// measurement ready
						controller_state.output_voltage=temp;
						controller_state.output_current=monitor.getCurrent();
						controller_state.timer_100ms=0;
						
						printstat(__LINE__);
						
						if ((RegulatorState.RegulatorStep==REGULATOR_HOLD) ||
							(RegulatorState.RegulatorStep==REGULATOR_OFF))
							monitor.clear_ready_flag();
						
						printstat(__LINE__);
					}	// measurement ready
					else
					{	// measurement not ready, wait
						controller_state.timer_100ms=90;
						
						printstat(__LINE__);
					}	// measurement not ready, wait
				}	// 100ms timer
			}	// check time dependend operations
			
			if (controller_state.evaluate_settings)
			{	// read back pd settings when ready
				if (controller_state.power_is_ready)
				{	// transition ready
					printstat(__LINE__);
					
					controller_state.evaluate_settings=false;
					if (!((PD.get_ps_status()==STATUS_POWER_PPS) &&
						  (controller_state.operating_mode!=CONTROLLER_MODE_OFF)))
					{	// no active PPS regulation
						printstat(__LINE__);
						
						controller_state.set_output_voltage=PD.get_voltage_mV();
						controller_state.set_output_current=PD.get_current_mA();
						
						printstat(__LINE__);
					}	// no active PPS regulation
				}	// transition ready
			}	// read back pd settings when ready
			
			printstat(__LINE__);
			break;
		default:
			printstat(__LINE__);
			CONTROLLER_DEBUG_TIME2();
			CONTROLLER_DEBUG_PRINTLN2(F("CONTROLLER default > error"));
			printstat(__LINE__);
			break;
	}	// switch main_state
	
	PD.run();
	#ifdef PD_UFP_LOG
		PD.print_status(Serial1);
	#endif
	
	
	controller_state.last_process =
		controller_state.process_time;

	printstat(__LINE__);

}	// process

bool controller_c::is_up(void)
{	// is_up
	return controller_state.main_state==CONTROLLER_RUN;
}	// is_up

bool controller_c::is_power_ready(void)
{	// is_up
	return 	controller_state.power_is_ready &&
			(!( RegulatorState.set_point_update ||
				RegulatorState.update_in_progress));
}	// is_up

bool controller_c::is_PPS(void)
{	// is_PPS
	return PD.get_ps_status()==STATUS_POWER_PPS;
}	// is_PPS

bool controller_c::is_PD(void)
{	// is_PPS
	return PD.get_ps_status()!=STATUS_POWER_NA;
}	// is_PPS


bool controller_c::is_constant_current_active(void)
{	// is_constant_current_active
	return RegulatorState.IntegralOffsetI!=0;
}	// is_constant_current_active


void controller_c::set_operating_mode(operating_mode_et operating_mode)
{	// set_operating_mode
	TODO(change operating mode)
	if (PD.get_ps_status()!=STATUS_POWER_PPS)
	{	// fix pdo, change in operating mode has no effect
		controller_state.operating_mode=operating_mode;
	}	// fix pdo, change in operating mode has no effect
	else if (controller_state.operating_mode!=operating_mode)
	{	
		controller_state.operating_mode=operating_mode;
		resetRegulator();
		enable_output(false);
	}
}	// set_operating_mode


void controller_c::set_voltage(uint16_t mV)
{	// set_voltage
	if (PD.get_ps_status()!=STATUS_POWER_TYP)
		enable_output(false);	// mode change, disable output
		
	set_PD(mV,0); // to set fix profile, set current to 0;
}	// set_voltage

void controller_c::set_power(uint16_t mV,uint16_t mA)
{	// set_current
	CONTROLLER_DEBUG_TIME2();
	CONTROLLER_DEBUG_PRINT2(" set_power =>");
	CONTROLLER_DEBUG_PRINTLN2(" Check Output");
	
	if (PD.get_ps_status()!=STATUS_POWER_PPS)
		enable_output(false);	// mode change, disable output
		
	CONTROLLER_DEBUG_TIME2();
	CONTROLLER_DEBUG_PRINT2(" set_power =>");
	CONTROLLER_DEBUG_PRINTLN2(" Check Current");
	
	if (mA>get_max_current_mA(mV)) mA=get_max_current_mA(mV);
	
	if ( (is_output_enabled()) && 
		 (controller_state.operating_mode !=
			CONTROLLER_MODE_OFF) )
	{	//	regulator is active, changes have to be set bei algorithm
		CONTROLLER_DEBUG_TIME2();
		CONTROLLER_DEBUG_PRINT2(" set_power =>");
		CONTROLLER_DEBUG_PRINTLN2(" Update Settings");

		RegulatorState.set_point_update=
			(controller_state.set_output_current!=mA) ||
			(controller_state.set_output_voltage!=mV);
		controller_state.new_output_current=mA;
		controller_state.new_output_voltage=mV;
	}	//	regulator is active, changes have to beset bei algorithm
	else
	{	//	regulator is inactive, just set new value	
		CONTROLLER_DEBUG_TIME2();
		CONTROLLER_DEBUG_PRINT2(" set_power =>");
		CONTROLLER_DEBUG_PRINTLN2(" set PD");

		RegulatorState.set_point_update=false;
		set_PD(mV,mA);
	}	//	regulator is inactive, just set new value	
}	// set_current

void controller_c::enable_output(bool bEnable)
{	// enable_output
	CONTROLLER_DEBUG_PRINT2(controller_state.process_time);
	CONTROLLER_DEBUG_PRINT2(F(" enableOutput="));
	CONTROLLER_DEBUG_PRINTLN2(bEnable);
	
	set_output_switch(bEnable);
	enableRegulator();
	if (!bEnable) 
		power_led.set_led_color(LED_POWER_OFF);	// outupt off -> LED off
	else if (PD.get_ps_status()!=STATUS_POWER_PPS)
		power_led.set_led_color(LED_POWER_FIX);	// no PPS -> mode "fix"
	else if (controller_state.operating_mode!=CONTROLLER_MODE_OFF)
		power_led.set_led_color(LED_POWER_CV);	// regulator on -> mode "CVCC"
	else
		power_led.set_led_color(LED_POWER_PPS);	// otherwise just "pps" mode
}	// enable_output

controller_c::operating_mode_et controller_c::get_operating_mode(void)
{	// get_operating_mode
	return controller_state.operating_mode;
}	// get_operating_mode

uint16_t controller_c::get_set_voltage(void)
{	// get_set_voltage
	uint16_t voltage=PD_STARTUP_VOLTAGE;

	// evaluate only if PD Source is available
	if (is_up()) 
	{	// is_up
		switch (controller_state.operating_mode)
		{	// switch operating mode
			case CONTROLLER_MODE_CV:
				/* fall trough */
			case CONTROLLER_MODE_CVCC:
				/* fall trough */
			case CONTROLLER_MODE_CVCCmax:
				voltage=controller_state.set_output_voltage;
				break;
			case CONTROLLER_MODE_OFF:
				/* fall trough */
			default:
				voltage=PD.get_voltage_mV();
				break;
		}	// switch operating mode
	}	// is_up
		
	return voltage;
}	// get_set_voltage

uint16_t controller_c::get_set_current(void)
{	// get_set_current
	uint16_t current=PD_NO_PD_CURRENT;

	// evaluate only if PD Source is available
	if (is_up()) 
	{	// is_up
		switch (controller_state.operating_mode)
		{	// switch operating mode
			case CONTROLLER_MODE_CV:
				/* fall trough */
			case CONTROLLER_MODE_CVCC:
				/* fall trough */
			case CONTROLLER_MODE_CVCCmax:
				current=controller_state.set_output_current;
				break;
			case CONTROLLER_MODE_OFF:
				/* fall trough */
			default:
				current=PD.get_current_mA();
				break;
		}	// switch operating mode
	}	// is_up

	return current;
}	// get_set_current

bool controller_c::is_output_enabled(void)
{	// is_output_enabled
	return controller_state.output_switch;
}	// is_output_enabled


uint16_t controller_c::get_min_voltage_mV(void) 		// minimum adjustable voltage
{	// get_min_voltage_mV
	uint16_t voltage=PD_STARTUP_VOLTAGE;
	PD_power_info_t Info;

	// evaluate only if PD Source is available
	if (is_up()) 
	{	// is_up
		voltage=UINT16_MAX;
		uint8_t Cnt=0;
		while (PD.get_PDO_info(Cnt++,&Info))
		{	// scan all profiles
			// evaluate only PPS
			if ( (Info.type==PD_PDO_TYPE_AUGMENTED_PDO ) &&
				 (Info.min_v<voltage) )
				voltage=Info.min_v;
		}	// scan all profiles
	}	// is_up

	return voltage;
}	// get_min_voltage_mV

uint16_t controller_c::get_max_voltage_mV(uint16_t mA) 	// maximum adjustable voltage at mA
{	// get_max_voltage_mV
	uint16_t voltage=PD_STARTUP_VOLTAGE;
	PD_power_info_t Info;

	// evaluate only if PD Source is available
	if (is_up()) 
	{	// is_up
		voltage=0;
		uint8_t Cnt=0;
		while (PD.get_PDO_info(Cnt++,&Info))
		{	// scan all profiles
			// evaluate only PPS
			if ( (Info.type==PD_PDO_TYPE_AUGMENTED_PDO ) &&
				 (Info.max_v>=voltage) && (Info.max_i>=mA) )
				voltage=Info.max_v;
		}	// scan all profiles
	}	// is_up

	return voltage;
}	// get_max_voltage_mV

uint16_t controller_c::get_max_current_mA(uint16_t mV) 	// maximum available current at mV
{	// get_max_current_mA
	uint16_t current=PD_NO_PD_CURRENT;
	PD_power_info_t Info;

	// evaluate only if PD Source is available
	if (is_up()) 
	{	// is_up
		current=0;
		uint8_t Cnt=0;
		while (PD.get_PDO_info(Cnt++,&Info))
		{	// scan all profiles
			// evaluate only PPS
			if ( (Info.type==PD_PDO_TYPE_AUGMENTED_PDO ) &&
				 (Info.max_v>=mV) && (Info.max_i>=current) )
				current=Info.max_i;
		}	// scan all profiles
	}	// is_up

	return current;
}	// get_max_current_mA

uint16_t controller_c::get_min_current_mA(void)			// min current depending on mode
{	// get_min_current_mA
	uint16_t minCurrent=0;	

	// according to spec, current limiting works above 1A
	// althoug smaller Values can be set
	// in CC mode min current is set to 10 mA
	// less will be tricky to maintain in CC mode 
	switch(controller_state.operating_mode)
	{	// switch operting_mode
		case CONTROLLER_MODE_OFF:
		/* fall through */
		case CONTROLLER_MODE_CV:
			minCurrent=100;
			break;
		case CONTROLLER_MODE_CVCC:
		/* fall through */
		case CONTROLLER_MODE_CVCCmax:
			minCurrent=10;
			break;
		default:
			break;
	}	// switch operting_mode
	return minCurrent;
}	// get_min_current_mA


uint16_t controller_c::get_fix_voltage_mV(uint16_t mV)	// closesest fix profile  
{	// get_fix_voltage_mv
	uint16_t voltage=PD_STARTUP_VOLTAGE;
	PD_power_info_t Info;

	// evaluate only if PD Source is available
	if (is_up()) 
	{	// is_up
		voltage=0;
		uint8_t Cnt=0;
		while (PD.get_PDO_info(Cnt++,&Info))
		{	// scan all profiles
			// evaluate only FIX
			if ( (Info.type==PD_PDO_TYPE_FIXED_SUPPLY ) &&
				 (Info.max_v<=mV))
				voltage=Info.max_v;
		}	// scan all profiles
	}	// is_up

	return voltage;
}	// get_fix_voltage_mv

uint8_t controller_c::get_current_profile(void)
{	// get_current_profile
	return PD.get_selected_PDO();
}	// get_current_profile

uint8_t controller_c::get_profile_cnt(void)
{	// get_profile_cnt
	return PD.get_PDO_cnt();
}	// get_profile_cnt

bool controller_c::get_profile_info(uint8_t profile, PD_power_info_t * pPowerInfo)
{	// get_profile_info
	return PD.get_PDO_info(profile,pPowerInfo);
}	// get_profile_info

uint16_t controller_c::get_Vbus_mV(void)
{	// get_Vbus_mV
	return Vbus.get();
}	// get_Vbus_mV

uint16_t controller_c::get_output_voltage_mV(void)
{	// get_output_voltage
	return controller_state.output_voltage;
}	// get_output_voltage

int16_t controller_c::get_output_current_mA(void)
{	// get_output_current
	return controller_state.output_current;
}	// get_output_current

void controller_c::set_output_current_calibration(uint16_t I_internal_val_mA, uint16_t I_reference_val_mA)
{	//	set_output_current_calibration
	uint32_t Cal=monitor.getCalibration();
	Cal*=I_reference_val_mA;
	Cal/=I_internal_val_mA;
	monitor.setCalibration(Cal);
}	//	set_output_current_calibration

void controller_c::reset_output_current_calibration(void)
{	// reset_output_current_calibration
	monitor.setCalibration(ADC_SHUNT_mR,ADC_CURRENT_LSB_mA,ADC_MAX_CURRENT_mA);		
}	// reset_output_current_calibration

const char * controller_c::get_profile_text(enum PD_power_data_obj_type_t profile_type) // get text of selected profile
{	// getProfileText
	switch (profile_type)
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
	


/* *********************************************************************
 * private functions
 */

void controller_c::init_power_led(void)
{	// init_power_led
	power_led.init();
	power_led.set_led_color(LED_STARTUP);
}	// init_power_led

void controller_c::init_output_switch(void)
{	// init_output_switch
	digitalWrite(OUTPUT_SWITCH_PIN,LOW);
	pinMode(OUTPUT_SWITCH_PIN,OUTPUT);
	set_output_switch(false);
}	// init_output_switch

void controller_c::init_PD(void)
{	// init_PD
	#ifdef ARDUINO_ARCH_AVR
		PD.clock_prescale_set(2);
		clock_prescale_set(clock_div_2);
	#endif
	PD.set_int_pin(PD_INT_PIN);
	PD.set_fallback_power_option(PD_FALLBACK_POWER_OPTION);
	PD.init_PD_PPS(PD_STARTUP_VOLTAGE);
	controller_state.evaluate_settings=true;
	start_check_PD_transition();
}	// init_PD

void controller_c::init_Vbus_adc(void)
{	// init_Vbus_adc
	#if defined( PD_PPS_CONTROLLER)
		analogReadResolution(12);
		analogReference(AR_INTERNAL1V0);
	#elif defined( PD_PPS_CONTROLLER_MINI)
		analogReadResolution(12);
		analogReference(AR_EXTERNAL);
	#endif
}	// init_Vbus_adc

bool controller_c::init_monitor(void)
{	// init_monitor
	bool bDone=false;
	switch (controller_state.monitor_state)
	{	// switch monitor_state
		case MONITOR_SET_BUS_VOLTAGE_RANGE:
			#ifdef HWCFG_ADC_TYPE_INA219
				monitor.setBusVoltageRange(ADC_BUS_VOLTAGE_RANGE);
			#else
				monitor.setAdcAveraging(ADC_AVERAGE_SLOW);
			#endif
			controller_state.monitor_state=MONITOR_SET_BUS_VOLTAGE_RESOLUTION;
			break;
		case MONITOR_SET_BUS_VOLTAGE_RESOLUTION:
			#ifdef HWCFG_ADC_TYPE_INA219
				monitor.setAdcAveraging(ADC_VOLTAGE_SELECT,ADC_AVERAGE_SLOW);
			#else
				monitor.setAdcConversionTime(ADC_VOLTAGE_SELECT,ADC_CONVERSION_TIME);
			#endif
			controller_state.monitor_state=MONITOR_SET_SHUNT_VOLTAGE_RESOLUTION;
			break;
		case MONITOR_SET_SHUNT_VOLTAGE_RESOLUTION:
			#ifdef HWCFG_ADC_TYPE_INA219
				monitor.setAdcAveraging(ADC_CURRENT_SELECT,ADC_AVERAGE_SLOW);
			#else
				monitor.setAdcConversionTime(ADC_CURRENT_SELECT,ADC_CONVERSION_TIME);
			#endif
			controller_state.monitor_state=MONITOR_SET_SHUNT_VOLTAGE_GAIN;
			break;
		case MONITOR_SET_SHUNT_VOLTAGE_GAIN:
				monitor.setShuntGain(monitor.calcSuntGain(ADC_SHUNT_mR,ADC_MAX_CURRENT_mA));
			controller_state.monitor_state=MONITOR_SET_MODE;
			break;
		case MONITOR_SET_MODE:
			monitor.setOperationMode(ADC_OPERATION_MODE);
			controller_state.monitor_state=MONITOR_SET_CALIBRATION;
			break;
		case MONITOR_SET_CALIBRATION:
			monitor.setCalibration(ADC_SHUNT_mR,ADC_CURRENT_LSB_mA,ADC_MAX_CURRENT_mA);		
			controller_state.monitor_state=MONITOR_WAIT_FIRST;
			break;
		case MONITOR_WAIT_FIRST:
			if (monitor.conversion_ready(&controller_state.output_voltage))
				controller_state.monitor_state=MONITOR_READ;
			break;
		case MONITOR_READ:
			controller_state.output_current=monitor.getCurrent();
			controller_state.monitor_state=MONITOR_INIT_END;
		break;
		case MONITOR_INIT_END:
			bDone=true;
			break;
		default:
			controller_state.monitor_state=MONITOR_SET_BUS_VOLTAGE_RANGE;
			break;
	}	// switch monitor_state
	return bDone;
}	// init_monitor

uint16_t controller_c::calc_Vbus(void)
{	// calc_Vbus
	/* **************************************
	 * MACRO: PD_PPS_CONTROLLER
	 * Vbus measured with 12 bit resolution
	 * at Vref=1V and 1:32 reduction
	 * => 1bit  = 1/128 V 
	 * adc val *1000/128 => 125/16 	5^3 / 2^4
	 * 								5=2^2,322
	 * 
	 * maximum reading is 21V = 2688 2^11,392
	 *
	 * 
	 * on less than 32bit arch it might be quicker to
	 * avoid 32 bit operations an stick to 16 bit:
	 * 4bits left to operate, to get max accuracy
	 * *5 =>+2,322 bits => 13,714 bit
	 * /2 =>-1 bit		=> 12,714 bit
	 * *5 =>+2,322 bits => 15,036 bit
	 * /4 =>-2 bit		=> 13,036 bit
	 * *5 =>+2,322 bits => 15,357 bit
	 * /2 =>-1 bit		=> 14,357 bit
	 * 
	 * 
	 ***************************************************** 	 
	 * MACRO: PD_PPS_CONTROLLER_MINI
	 * Vbus measured with 12 bit resolution
	 * at Vref=2.048V and 1/7! reduction Umax~14V
	 * => 1bit  = 3.5mV V 
	 * adc val *3,5 => 7/2
	 * 
	 * 
	 */

	uint32_t voltage=analogRead(VBUS_PIN);
	#if defined(PD_PPS_CONTROLLER)
		voltage*=125;
		voltage>>=4;
	#elif defined(PD_PPS_CONTROLLER_MINI)
		voltage*=7;
		voltage>>=1;
	#else
		#error undefined configuration
	#endif
	return (uint16_t)voltage;
}	// calc_Vbus

/* *********************************************************************
 * PD source controll functions
 */
 
void controller_c::set_output_switch(bool bOn)
{	// set_output_switch
	digitalWrite(OUTPUT_SWITCH_PIN,bOn);
	controller_state.output_switch=bOn;
}  // set_output_switch


void controller_c::set_PD(uint16_t voltage, uint16_t current)
{	// set new pd / pps profile / data
	uint16_t set_current=current;
	CONTROLLER_DEBUG_PRINT2(controller_state.process_time);
	CONTROLLER_DEBUG_PRINT2(F(" setPDPPS: U="));
	CONTROLLER_DEBUG_PRINT2(voltage);
	CONTROLLER_DEBUG_PRINT2(F(" I="));
	CONTROLLER_DEBUG_PRINT2(current);
	if (current==0)
	{	// new profile is fixed
		if ( (PD.get_ps_status() == STATUS_POWER_PPS) ||
			 (PD.calc_power_option(controller_state.set_output_voltage) !=
				PD.calc_power_option(voltage) ) )
		{	// change to fix or change in voltage
			CONTROLLER_DEBUG_PRINT2(" FIX ");
			PD.set_PD_PPS(voltage);
			start_check_PD_transition();
		}	// change to fix or change in voltage
	}	// new profile is fixed
	else
	{	// new profile is PPS
		if (PD.get_ps_status() == STATUS_POWER_TYP)
		{	// change to PPS profile, reinit
			CONTROLLER_DEBUG_PRINTLN2(F(" INIT PPS"));
			if (controller_state.operating_mode==CONTROLLER_MODE_CVCCmax)
			{
				set_current=get_max_current_mA(voltage);
				CONTROLLER_DEBUG_PRINT2(F(" Imax="));
				CONTROLLER_DEBUG_PRINT2(set_current);
			}
			PD.init_PD_PPS(voltage,set_current);
			start_check_PD_transition();
		}	// change to PPS profile, reinit
		else
		{	// allready PPS
			if (controller_state.operating_mode==CONTROLLER_MODE_CVCCmax)
			{
				set_current=get_max_current_mA(voltage);
				CONTROLLER_DEBUG_PRINT2(F(" Imax="));
				CONTROLLER_DEBUG_PRINT2(set_current);
			}
			if (PD.set_PD_PPS(voltage,set_current))
				start_check_PD_transition();	// if changes, check
		}	// allready PPS
	}	// new profile is PPS

	CONTROLLER_DEBUG_PRINT2(F("\n"));

	controller_state.set_output_voltage=voltage;
	controller_state.set_output_current=current;

	// unregulated operation -> read back parameter
	controller_state.evaluate_settings= 
		(PD.get_ps_status() == STATUS_POWER_TYP) ||
		 (controller_state.operating_mode==CONTROLLER_MODE_OFF);
	//resetRegulator();
}	// set new pd / pps profile / data

void controller_c::start_check_PD_transition(void)
{	// start_check_PD_transition
	if (controller_state.power_is_ready)
	{	// power has been preveously established, reset
		controller_state.power_is_ready=false;
		controller_state.power_transition_timout=false;
		controller_state.power_transition_timer=
			controller_state.process_time;
		controller_state.pd_transition_state=PD_TRANSITION_START;
	}	// power has been preveously established, reset
}	// start_check_PD_transition

void controller_c::check_PD_transition(void)
{	// is Pd ready
	switch (controller_state.pd_transition_state)
	{	// check power transition
		case PD_TRANSITION_START:
			if (PD.is_ps_transition() || 
				controller_state.power_transition_timout) 
			{	// transition start
				controller_state.pd_transition_state=
					PD_TRANSITION_WAIT_TRANSITION;
				controller_state.power_transition_timer=
					controller_state.process_time;
			}	// transition start
			else 
			{	// check timeout
				controller_state.power_transition_timout=
					((uint16_t)(controller_state.process_time - 
								controller_state.power_transition_timer) >
					 PD_TRANSITION_TIMOUT); 
			}	// check timeout
			break;
		case PD_TRANSITION_WAIT_TRANSITION:
			if (!PD.is_ps_transition() ||
				controller_state.power_transition_timout)
			{	// transition completed
				controller_state.pd_transition_state=
					PD_TRANSITION_WAIT_POWER;
				controller_state.power_transition_timer=
					controller_state.process_time;
			}	// transition completed
			else 
			{	// check timeout
				controller_state.power_transition_timout=
					((uint16_t)(controller_state.process_time - 
								controller_state.power_transition_timer) >
					 PD_TRANSITION_TIMOUT); 
			}	// check timeout
			break;
		case PD_TRANSITION_WAIT_POWER:	
			if ((PD.is_power_ready() && !PD.is_PPS_ready()) ||
				(!PD.is_power_ready() && PD.is_PPS_ready()) ||
				controller_state.power_transition_timout)
			{	// power ok, final delay
				controller_state.pd_transition_state=PD_TRANSITION_WAIT_FINAL;
			}	// power ok, final delay
			else 
			{	// check timeout
				controller_state.power_transition_timout=
					((uint16_t)(controller_state.process_time - 
								controller_state.power_transition_timer) >
					 PD_TRANSITION_TIMOUT); 
			}	// check timeout
			break;
		case PD_TRANSITION_WAIT_FINAL:
			// not yet implemented
			controller_state.pd_transition_state=PD_TRANSITION_READY;
			break;
		case PD_TRANSITION_READY:
			controller_state.power_is_ready=true;
			break;
	}
}	// is Pd ready 


/* *********************************************************************
 * Regulator functions
 */
void controller_c::Regulator(void)
{	// regulator
	uint16_t temp;
	switch (RegulatorState.RegulatorStep)
	{	// regulator state machine
		case REGULATOR_START:
			if (PD.is_PPS_ready())
			{
				RegulatorState.update_in_progress=false;
				CONTROLLER_DEBUG_PRINT(F(" R="));
				CONTROLLER_DEBUG_PRINT(controller_state.process_time);
				RegulatorState.Timer=controller_state.process_time;
				RegulatorState.RegulatorStep=REGULATOR_WAIT_DELAY;
			}
			break;
		case REGULATOR_WAIT_DELAY:			
			if ((uint16_t)(controller_state.process_time-RegulatorState.Timer)>REG_DELAY)
			{
				CONTROLLER_DEBUG_PRINT(F(" D="));
				CONTROLLER_DEBUG_PRINT(controller_state.process_time);
				monitor.clear_ready_flag();
				RegulatorState.RegulatorStep=REGULATOR_MEASURE;
			}
			break;
		case REGULATOR_MEASURE:
			if (monitor.conversion_ready(&temp))
			{
				CONTROLLER_DEBUG_PRINT(F(" M="));
				CONTROLLER_DEBUG_PRINTLN(controller_state.process_time);
				controller_state.output_voltage=temp;
				controller_state.output_current=monitor.getCurrent();
				if (controller_state.output_current<0) controller_state.output_current=0;
				
				CONTROLLER_DEBUG_PRINT(F(" U="));
				CONTROLLER_DEBUG_PRINT(controller_state.output_voltage);
				CONTROLLER_DEBUG_PRINT(F(" I="));
				CONTROLLER_DEBUG_PRINT(controller_state.output_current);
				
				RegulatorState.RegulatorStep=REGULATOR_START;
				if (!RegulatorState.set_point_update)
				{	// normal operation, check deviation and calculate new controlvariable
					RegulatorCalc();
				}	// normal operation, check deviation and calculate new control variable
				else
				{	// new set point received
					RegulatorSetPoint();
				}	// new set point received
			}
			break;
		case REGULATOR_WAIT_TRANSITION_START:
			if(PD.is_ps_transition())
			{
				RegulatorState.RegulatorStep=REGULATOR_WAIT_TRANSITION_END;
				CONTROLLER_DEBUG_PRINT(F(" T="));
				CONTROLLER_DEBUG_PRINT(controller_state.process_time);
			}
			break;
		case REGULATOR_WAIT_TRANSITION_END:
			if(!PD.is_ps_transition())
			{
				RegulatorState.RegulatorStep=REGULATOR_START;
				CONTROLLER_DEBUG_PRINT(F(" E="));
				CONTROLLER_DEBUG_PRINT(controller_state.process_time);
			}
			break;
		default:
			// inactive, do nothing
			break;
	}	// regulator state machine
}	// regulator

void controller_c::RegulatorCalc(void)
{	// RegulatorCalc
	if (controller_state.operating_mode!=CONTROLLER_MODE_CV)
	{	// CVCC mode 
		if (controller_state.operating_mode==CONTROLLER_MODE_CVCC)
		{
			CONTROLLER_DEBUG_PRINT(" CVCC ");
		}
		else
		{
			CONTROLLER_DEBUG_PRINT(" CVCCmax ");
		}
		if (!RegulatorCC(controller_state.set_output_current-controller_state.output_current))
		{	// not in current limiting mode
			RegulatorCV(controller_state.set_output_voltage-controller_state.output_voltage);
		}	// not in current limiting mode
	}	// CVCC mode
	else
	{	// CV only, overcurrentprotection is done by PD source 
		CONTROLLER_DEBUG_PRINT(" CV ");
		if (controller_state.output_current<controller_state.set_output_current)
		{
			if (!RegulatorState.LockCount) 
			{	// not locked -> regulator calculation
				RegulatorCV(controller_state.set_output_voltage-controller_state.output_voltage);
				// if Regulator does not need to set new value,
				// LED will not be changed, need to be set seperately
				power_led.set_led_color(LED_POWER_CV);	
			}	// not locked -> regulator calculation
			else
			{	// locked -> wait
				CONTROLLER_DEBUG_PRINT(F(" CW="));
				CONTROLLER_DEBUG_PRINT(RegulatorState.LockCount);
				RegulatorState.LockCount--;
			}	// locked -> wait
		}
		else
		{	// set lock count
			power_led.set_led_color(LED_POWER_CC);	
			RegulatorState.LockCount=CURRENT_LOCK_CYCLES;
		}	// set lock count
	}	// CV , overcurrentprotection is done by PD source
}	// RegulatorCalc


bool controller_c::RegulatorCC(int16_t delta)
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
			CONTROLLER_DEBUG_PRINT(F(" dI="));
			CONTROLLER_DEBUG_PRINT(delta);
			CONTROLLER_DEBUG_PRINT(F(" iU="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.IntegralOffsetU);
			CONTROLLER_DEBUG_PRINT(F(" iI="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.IntegralOffsetI);
		
			// clear min_delta in order to recalculate
			if (RegulatorState.IntegralStepI==0)
				RegulatorState.min_deltaI=0;
			
			// calculate relative step size
			Scale=1;
			while ((controller_state.set_output_current>>Scale)>abs(delta)) Scale++;
			relStep=controller_state.set_output_voltage>>Scale;

			CONTROLLER_DEBUG_PRINT(F(" Scale="));
			CONTROLLER_DEBUG_PRINT(Scale);
			CONTROLLER_DEBUG_PRINT(F(" relStep="));
			CONTROLLER_DEBUG_PRINT(relStep);
			
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

			CONTROLLER_DEBUG_PRINT(F(" ldI="));
			CONTROLLER_DEBUG_PRINT(lastDelta);
			
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
							
			OffsetLimit=get_min_voltage_mV();
			OffsetLimit-=controller_state.set_output_voltage+RegulatorState.IntegralOffsetU;
							
			if (RegulatorState.IntegralOffsetI<OffsetLimit)
			{	// Voltage Reduction to high, start fuse counter
				RegulatorState.FuseCount++;
				RegulatorState.IntegralOffsetI=OffsetLimit;	
				if (RegulatorState.FuseCount>=FUSE_LIMIT)
				{	// fuse limit reached, turn off power
					enable_output(false);
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

			CONTROLLER_DEBUG_PRINT(F(" Istep="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.IntegralStepI);
			CONTROLLER_DEBUG_PRINT(F(" iI="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.IntegralOffsetI);
			CONTROLLER_DEBUG_PRINT(F(" osc="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.OscillationCount);
			CONTROLLER_DEBUG_PRINT(F(" msum="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.min_deltaI_sum);
			CONTROLLER_DEBUG_PRINT(F(" minD="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.min_deltaI);
			CONTROLLER_DEBUG_PRINT(F(" Cycle="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.CycleCount);
			CONTROLLER_DEBUG_PRINT(F(" Fuse="));
			CONTROLLER_DEBUG_PRINTLN(RegulatorState.FuseCount);

			
			RegulatorSet();
		}	// current deviation to high
		return true;
	}	// start constant current regulation
	else
		return false; 
}	// calculate constant current

bool controller_c::RegulatorCV(int16_t delta)
{	// calculate constant voltage
	int16_t OffsetLimit;
	if (abs(delta)>MIN_DELTA_U)
	{	// voltage difference , increase Setting
		if ( (abs(RegulatorState.IntegralOffsetU)<MAX_INTEGRAL_U) ||	// not yet saturated
			 ( (RegulatorState.IntegralOffsetU>0) && (delta<0) ) || 	// overcompensated 
			 ( (RegulatorState.IntegralOffsetU<0) && (delta>0) ) )		// undercompensated
		{	// Integral Offset within Limits
			CONTROLLER_DEBUG_PRINT(F(" dU="));
			CONTROLLER_DEBUG_PRINT(delta);


			RegulatorState.IntegralOffsetU+=delta;

			
			// max. Offset to lowest possible voltage
			OffsetLimit=get_min_voltage_mV();
			OffsetLimit-=controller_state.set_output_voltage;
			if (RegulatorState.IntegralOffsetU<OffsetLimit)
			{	// new offset sets voltage below min voltage
				RegulatorState.IntegralOffsetU=OffsetLimit;
			}	// new offset sets voltage below min voltage
			
			// max. Offset to highest possible voltage
			OffsetLimit=get_max_voltage_mV(controller_state.set_output_current);
			OffsetLimit-=controller_state.set_output_voltage;
			if (RegulatorState.IntegralOffsetU>OffsetLimit)
			{	// new offset sets voltage above max voltage
				RegulatorState.IntegralOffsetU=OffsetLimit;
			}	// new offset sets voltage above max voltage
			
			CONTROLLER_DEBUG_PRINT(F(" iU="));
			CONTROLLER_DEBUG_PRINT(RegulatorState.IntegralOffsetU);

			RegulatorSet();
		}	// Integral Offset within Limits
		else
		{	// saturated, just wait
			CONTROLLER_DEBUG_PRINT(" dU:SAT ");
		}	// saturated, just wait
		return true;
	}	// voltage difference , increase Setting
	else
	{
		CONTROLLER_DEBUG_PRINT(" dU<MDU ");
		return false;
	}
}	// calculate constant voltage

void controller_c::RegulatorSetPoint(void)
{	// RegulatorSetPoint
	// check if current limiting is active
	CONTROLLER_DEBUG_PRINT(F(" SET "));
	
	RegulatorState.set_point_update=false;
	RegulatorState.update_in_progress=true;
	
	if (RegulatorState.IntegralOffsetI!=0)
	{	// current limiting is active
		RegulatorState.IntegralOffsetI-=
			controller_state.new_output_voltage-
			controller_state.set_output_voltage;
	}	// current limiting is active
	
	if (controller_state.new_output_voltage +
		RegulatorState.IntegralOffsetI+
		RegulatorState.IntegralOffsetU>get_max_voltage_mV(0))
	{	// control variable execceds limit
		RegulatorState.IntegralOffsetU=
			get_max_voltage_mV(0)-
			controller_state.new_output_voltage -
			RegulatorState.IntegralOffsetI;
	}	// control variable execceds limit
	
	controller_state.set_output_current=
		controller_state.new_output_current;
	controller_state.set_output_voltage=
		controller_state.new_output_voltage;
	
	RegulatorSet();
}	// RegulatorSetPoint

void controller_c::RegulatorSet(void)
{	// set new voltage
	uint16_t uPPS;
	uint16_t iPPS;
	TODO(Aenderung des stroms beachten)
	if (controller_state.output_switch)
	{	// power is active, set new value
		CONTROLLER_DEBUG_PRINT2(controller_state.process_time);
		CONTROLLER_DEBUG_PRINT2(F(" RegulatorSet: "));
		uPPS=controller_state.set_output_voltage;
		uPPS+=RegulatorState.IntegralOffsetU;
		uPPS+=RegulatorState.IntegralOffsetI;
		/* in case of multiple APDOs, differen max. current limits
		 * across the voltage range may be possible
		 */
		iPPS=get_max_current_mA(uPPS);
		if (iPPS<controller_state.set_output_current)
		{	// requested current exceeds max. available current 
			// reduce requested current 
			controller_state.set_output_current=iPPS;
		}	// requested current exceeds max. available current
		else if (controller_state.operating_mode!=CONTROLLER_MODE_CVCCmax)
		{	// max. available current exceeds requested current
			// in CV or CVCC mode use requested current
			iPPS=controller_state.set_output_current;
		}	// max. available current exceeds requested current

		if (PD.set_PD_PPS(uPPS,iPPS))
		{	// new values, start wait for PD source
			RegulatorState.RegulatorStep=REGULATOR_WAIT_TRANSITION_START;

			CONTROLLER_DEBUG_PRINT2(F("Ustep="));
			CONTROLLER_DEBUG_PRINT2(uPPS);
			CONTROLLER_DEBUG_PRINT2(F(" Istep="));
			CONTROLLER_DEBUG_PRINT2(iPPS);
			CONTROLLER_DEBUG_PRINT(F(" S="));
			CONTROLLER_DEBUG_PRINT(controller_state.process_time);
		}	// new values, start wait for PD source
		else
		{	// no change
			CONTROLLER_DEBUG_PRINT(F(" NC"));
		}	// no change

		/*
		if (uPPS!=RegulatorState.lastVoltageSet)
		{	// set only if different to last value
			RegulatorState.lastVoltageSet=uPPS;


			if (controller_state.operating_mode!=CONTROLLER_MODE_CVCCmax)
			{	// use power supply for additional current limiting
				CONTROLLER_DEBUG_PRINT2(F("Ustep="));
				CONTROLLER_DEBUG_PRINT2(uPPS);
				CONTROLLER_DEBUG_PRINT2(F(" Istep="));
				CONTROLLER_DEBUG_PRINT2(iPPS);
				PD.set_PD_PPS(uPPS,iPPS);
			}	// use power supply for additional current limiting
			else
			{	// set max current, to disable current limiting from power supply
				PD.set_PD_PPS(uPPS,get_max_current_mA(uPPS));	
				CONTROLLER_DEBUG_PRINT2(F("Ustep="));
				CONTROLLER_DEBUG_PRINT2(uPPS);
				CONTROLLER_DEBUG_PRINT2(F(" Istep="));
				CONTROLLER_DEBUG_PRINT2(get_max_current_mA(uPPS));
			}
			
			RegulatorState.RegulatorStep=REGULATOR_WAIT_TRANSITION_START;
			CONTROLLER_DEBUG_PRINT(F(" S="));
			CONTROLLER_DEBUG_PRINT(controller_state.process_time);
		}	// set only if different to last value
		*/
		if (RegulatorState.IntegralOffsetI)
			power_led.set_led_color(LED_POWER_CC);
		else
			power_led.set_led_color(LED_POWER_CV);
	}	// power is active, set new value
}	// set new voltage

void controller_c::enableRegulator(void)
{	// enable Regulator
	CONTROLLER_DEBUG_PRINT2(controller_state.process_time);
	CONTROLLER_DEBUG_PRINT2(F(" enable Regulator: "));
	TODO(operating_mode != CONTROLLER_MODE OFF && Fixed profile)
	if ((controller_state.operating_mode==CONTROLLER_MODE_OFF) ||
		(PD.get_ps_status()!=STATUS_POWER_PPS))
	{	// no regulation or FIX profile
		CONTROLLER_DEBUG_PRINT2(F("off, ADC slow"));
		resetRegulator();
		RegulatorState.RegulatorStep=REGULATOR_OFF;
		#ifdef HWCFG_ADC_TYPE_INA219
			monitor.setAdcAveraging(ADC_VOLTAGE_SELECT,ADC_AVERAGE_SLOW);
			monitor.setAdcAveraging(ADC_CURRENT_SELECT,ADC_AVERAGE_SLOW);
		#else
			monitor.setAdcAveraging(ADC_AVERAGE_SLOW);
		#endif
	}	// no regulation or FIX profile
	else
	{	// active
		CONTROLLER_DEBUG_PRINT2(F("ADC fast "));
		#ifdef HWCFG_ADC_TYPE_INA219
			monitor.setAdcAveraging(ADC_VOLTAGE_SELECT,ADC_AVERAGE_REGULATOR);
			monitor.setAdcAveraging(ADC_CURRENT_SELECT,ADC_AVERAGE_REGULATOR);
		#else
			monitor.setAdcAveraging(ADC_AVERAGE_REGULATOR);
		#endif
		switch (RegulatorState.RegulatorStep)
		{	// decision depending on current State
			case REGULATOR_START:
			case REGULATOR_MEASURE:
			case REGULATOR_WAIT_TRANSITION_START:
			case REGULATOR_WAIT_TRANSITION_END:
			case REGULATOR_WAIT_DELAY:			
				// if active: no Output -> just disable
				if (!controller_state.output_switch)
				{
					CONTROLLER_DEBUG_PRINT2(F(" disable"));
					resetRegulator();
					RegulatorState.RegulatorStep=REGULATOR_HOLD;
				}
				break;
			case REGULATOR_OFF:
				CONTROLLER_DEBUG_PRINT2(F(" off -> hold"));
				RegulatorState.RegulatorStep=REGULATOR_HOLD;
				/* fall through */ 
			case REGULATOR_HOLD:
				// if inactive active: Output on -> just enable
				if (controller_state.output_switch)
				{
					CONTROLLER_DEBUG_PRINT2(F(" enable"));
					resetRegulator();
					// get last settings
					RegulatorState.lastVoltageSet=PD.get_voltage_mV();
					RegulatorState.IntegralOffsetU=RegulatorState.lastVoltageSet;
					RegulatorState.IntegralOffsetU-=controller_state.set_output_voltage;
				}
				break;
			default:
				RegulatorState.RegulatorStep=REGULATOR_OFF;
				break;
		}	// decision depending on current State
	}	// active
	CONTROLLER_DEBUG_PRINTLN2("");
}	// enable Regulator

void controller_c::resetRegulator(void)
{	// reset Regulator to start
	RegulatorState.RegulatorStep=REGULATOR_START;
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
	RegulatorState.set_point_update=false;
	RegulatorState.update_in_progress=false;
}	// reset Regulator to start
