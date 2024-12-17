#ifndef __controller_c_h__
#define __controller_c_h__




#include <stdint.h>
#include "PD_PPS.h"
#include "rgb_led.h"
#include "INA219.h"
#include "IIR.h"

class controller_c
{
	public:
		/* *************************************************************
		 * Operating modes:
		 * FIX:
		 * 	onlye fixed voltage profiles are used
		 * PPS:
		 * 	unregulatet PPS mode, voltage / current settings are directly
		 * 	transfered to the PD source
		 * CV:
		 * 	constant voltage mode, PD source voltage is adjustet to meet
		 *  requested voltage by +/-20 mV
		 * CVCC:
		 * 	extends CV mode with current limiting, current limit is set to
		 *  the PD source and output voltage is	adjusted in a way that 
		 *  drawn current is always below set limit
		 * 	any transient current spikes must be below the overload specs
		 *  of the PD source otherwise the PD source will switch off
		 * CVCCmax:
		 * 	same as CVCC but currentlimit is set to maximum availabe current
		 *  so current limiting and circuit braeker operations are perfomed
		 *  by the software and not by the PD source
		 */
		typedef enum:uint8_t { 	CONTROLLER_MODE_OFF,	
								CONTROLLER_MODE_CV,	
								CONTROLLER_MODE_CVCC,
								CONTROLLER_MODE_CVCCmax,
								CONTROLLER_MODE_end
								} operating_mode_et;
												
	
		controller_c(void);
		void process(void);
		bool is_up(void);
		bool is_power_ready(void);
		bool is_PPS(void);
		bool is_constant_current_active(void);
		
		uint16_t clock_ms(void) { return PD.clock_ms(); }			// alt: controller_state.process_time

		void set_operating_mode(operating_mode_et operating_mode);	// set regulator mode
		void set_voltage(uint16_t mV);								// set fiexed mode
		void set_power(uint16_t mV, uint16_t mA);					// set pps mode
		void enable_output(bool bEnable);
		bool is_output_enabled(void);

		operating_mode_et get_operating_mode(void);
		uint16_t get_set_voltage(void);
		uint16_t get_set_current(void);
		
		uint16_t get_min_voltage_mV(void);			// minimum adjustable voltage
		uint16_t get_max_voltage_mV(uint16_t mA);	// maximum adjustable voltage at mA
		uint16_t get_max_current_mA(uint16_t mV);	// maximum available current at mV
		uint16_t get_min_current_mA(void);			// min current depending on mode
		uint16_t get_fix_voltage_mV(uint16_t mV);	// closesest fix profile  

		uint8_t get_current_profile(void);
		uint8_t get_profile_cnt(void);
		bool get_profile_info(uint8_t profile, PD_power_info_t * pPowerInfo);

		uint16_t get_Vbus_mV(void);
		uint16_t get_output_voltage_mV(void);
		int16_t get_output_current_mA(void);
		
		void set_output_current_calibration(uint16_t I_internal_val_mA, uint16_t I_reference_val_mA);
		void reset_output_current_calibration(void);
		
		const char * get_profile_text(enum PD_power_data_obj_type_t profile_type);
		
	private:
		typedef enum:uint8_t {	CONTROLLER_INIT_PD,
								CONTROLLER_INIT_MONITOR,
								CONTROLLER_INIT_VBUS_IIR,
								CONTROLLER_RUN,
								CONTROLLER_end
								} controller_state_et;

		typedef enum:uint8_t {	MONITOR_SET_BUS_VOLTAGE_RANGE,
								MONITOR_SET_BUS_VOLTAGE_RESOLUTION,
								MONITOR_SET_SHUNT_VOLTAGE_RESOLUTION,
								MONITOR_SET_SHUNT_VOLTAGE_GAIN,
								MONITOR_SET_MODE,
								MONITOR_SET_CALIBRATION,
								MONITOR_WAIT_FIRST,
								MONITOR_READ,
								MONITOR_INIT_END,
								MONITOR_end
								} monitor_init_state_et;
			
		typedef enum:uint8_t { 	PD_TRANSITION_START,			// initialize wait
								PD_TRANSITION_WAIT_TRANSITION,	// wait uintil transition is finished
								PD_TRANSITION_WAIT_POWER,		// wait until power flag is stable
								PD_TRANSITION_WAIT_FINAL,		// wait before evaluation pd data
								PD_TRANSITION_READY
								} pd_transiton_et;
								
		typedef enum:uint8_t {	REGULATOR_START,				// clear valid flag of measurement
								REGULATOR_MEASURE,				// wait until measurement is ready
								REGULATOR_WAIT_TRANSITION_START,// wait until pps is in trasition
								REGULATOR_WAIT_TRANSITION_END,	// wait until pps transition finishe
								REGULATOR_WAIT_DELAY,			// delay before Measuring
								REGULATOR_HOLD,					// no action
								REGULATOR_OFF					// no regulation
								} regulator_et;
		
		typedef enum:uint8_t {	LED_POWER_OFF	=rgb_led_c::RGB_LED_OFF,	// power LED off
								LED_POWER_FIX	=rgb_led_c::RGB_LED_GREEN,	// power LED green
								LED_POWER_PPS	=rgb_led_c::RGB_LED_YELLOW,	// power LED yellow
								LED_POWER_CV	=rgb_led_c::RGB_LED_BLUE,	// power LED blue
								LED_POWER_CC	=rgb_led_c::RGB_LED_RED		// power LED red
								} led_power_et;

		static const char pgmTextPowerProfileFIX[] PROGMEM;
		static const char pgmTextPowerProfileBAT[] PROGMEM;
		static const char pgmTextPowerProfileVAR[] PROGMEM;
		static const char pgmTextPowerProfileAUG[] PROGMEM;
		static const char pgmTextPowerProfileNON[] PROGMEM;

			
		// regulator
		static const int16_t MIN_DELTA_U	=20;	// minimum delta for regulator
		static const int16_t MIN_DELTA_I	=10;	// minimum delta for regulator
		static const int16_t MAX_INTEGRAL_U	=3300;	// saturation value for integral U
		static const int16_t MIN_INCREMENT_U=20;	// minimum increment / decremnt on regulator voltage
		static const uint8_t FUSE_LIMIT		=3;		// max. 3 cycles at lowest voltage before power off
		static const uint8_t INCREMENT_LIMIT=3;		// after >3 successive increments increas integral step
		static const uint8_t MAX_OSC_I		=3;		// after >3 successive oscillations at MIN_INCREMENT stop regulator
		static const uint8_t CURRENT_LOCK_CYCLES=10;// wait n cycles after pps current limit before restarting regulation
		static const uint8_t REG_DELAY		=1;		// wait ...ms before measuring

		// definitions for INA219
		static const uint8_t						ADC_BASE_ADDRESS		= 0x40;
		static const INA219::bus_voltage_range_et	ADC_BUS_VOLTAGE_RANGE	= INA219::BUS_VOLTAGE_32V;
		static const INA219::gain_et				ADC_SHUNT_VOLTAGE_GAIN	= INA219::PG_div_2;
		static const uint8_t						ADC_AVERAGE_REGULATOR	= 0;
		static const uint8_t						ADC_AVERAGE_SLOW		= 7;
		static const INA219::op_mode_et				ADC_OPERATION_MODE		= INA219::MODE_ALL_CONTINOUS;
		static const uint16_t						ADC_CALIBRATION_VALUE	= 2731; // Shunt:0,015R / Resolution:1mA / FullRange: 80mV
																			// 410; // Shunt:0.1R 	/ Resolution:1mA / FullRange:320mV
		
		#ifdef ARDUINO_ARCH_AVR
			static const uint8_t POWER_LED_RED		=21;	// LED BuiltIn
			static const uint8_t POWER_LED_GREEN	=20;	// LED RX
			static const uint8_t POWER_LED_BLUE		=19;	// LED TX
			static const bool	 POWER_LED_OFF		=false;	// pin State for LED OFF
			
			static const uint8_t OUTPUT_SWITCH_PIN	=10;
		#else
			static const uint8_t POWER_LED_RED		=5;	
			static const uint8_t POWER_LED_GREEN	=3;
			static const uint8_t POWER_LED_BLUE		=4;
			static const bool	 POWER_LED_OFF		=true;	// pin State for LED OFF

			static const uint8_t OUTPUT_SWITCH_PIN	=2;
			static const uint8_t VBUS_PIN			=19;	// D19/A5/PB02 voltage divider vbus
		#endif
		
		static const uint8_t PD_INT_PIN=7;
		static const enum PD_power_option_t PD_FALLBACK_POWER_OPTION=PD_POWER_OPTION_MAX_5V;
		static const uint16_t PD_STARTUP_VOLTAGE=5000;
		static const uint16_t PD_NO_PD_CURRENT=1000;	// no information about PD source
		static const uint16_t PD_TRANSITION_TIMOUT=500;	// 500 ms transition timeout

		typedef struct controller_state_s { uint16_t				process_time;
											uint16_t				last_process;
											uint16_t				power_transition_timer;
											uint16_t				output_voltage;
											int16_t					output_current;
											uint16_t				set_output_voltage;
											uint16_t				set_output_current;
											uint16_t				new_output_voltage;
											uint16_t				new_output_current;
											uint8_t					timer_10ms;
											uint8_t					timer_100ms;
											controller_state_et 	main_state;
											operating_mode_et		operating_mode;
											monitor_init_state_et 	monitor_state;
											pd_transiton_et			pd_transition_state;
											bool					evaluate_settings;
											bool					output_switch;
											bool					power_is_ready;
											bool					power_transition_timout;
										}	controller_state_st;


		typedef struct regulator_s 		{	int16_t			IntegralOffsetU;	// integral part U
											int16_t			IntegralOffsetI;	// integral Par I
											int16_t			IntegralStepI;		// SAR for I regulation
											int16_t			lastPosDeltaI;		// value to check if SAR algorithm has to be restarted
											int16_t			lastNegDeltaI;		// value to check if SAR algorithm has to be restarted
											uint16_t		lastVoltageSet;		// reminder to check if new value has to be set
											uint16_t		Timer;				// delay between new regulator cycle and begin of measuring
											uint16_t		min_deltaI;			// dynamic calculated tolerance window when current regulation is stable
											uint16_t		min_deltaI_sum;		// accumulator for min_deltaI
											uint8_t			CycleCount;			// cycle counter for current regulator to increase step size
											uint8_t			FuseCount;			// cycle counter for current limiting below min volatage
											uint8_t			OscillationCount;	// cycle counter at minimum step size to check if regulation is stable
											uint8_t			LockCount;			// cycle counter befor restarting voltage regulation after PPS regulation
											bool			set_point_update;	// when output enabled new values are set by regulator algroithm
											bool			update_in_progress;	// reset when new setpoint is active
											regulator_et	RegulatorStep;		// state variable
											} regulator_st;

		PD_PPS_c 	PD;
		INA219		monitor{ADC_BASE_ADDRESS};
		rgb_led_c	power_led{POWER_LED_RED,POWER_LED_GREEN,POWER_LED_BLUE,POWER_LED_OFF};

		IIR<uint16_t,uint32_t> Vbus{4};
		
		controller_state_st 	controller_state;
		regulator_st			RegulatorState;


		void init_power_led(void);
		void init_output_switch(void);
		void init_PD(void);
		void init_Vbus_adc(void);
		bool init_monitor(void);

		void set_output_switch(bool bOn);
		void set_PD(uint16_t voltage, uint16_t current);
		void start_check_PD_transition(void);
		void check_PD_transition(void);
		uint16_t calc_Vbus(void);
		
		void Regulator(void);
		void RegulatorCalc(void);
		bool RegulatorCC(int16_t delta);
		bool RegulatorCV(int16_t delta);
		void RegulatorSetPoint(void);
		void RegulatorSet(void);
		void enableRegulator(void);
		void resetRegulator(void);
		
		void printstat(uint16_t Zeile);

};
#endif	//__controller_c_h__
