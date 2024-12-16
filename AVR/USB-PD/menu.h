#ifndef __menu__
#define __menu__

#include <stdint.h>
//#include <stdio.h>
#include "parameter.h"
#include "lcd.h"
#include "INA219.h"
#include "PD_PPS.h"
#include "IIR.h"

class menu
{
	public:
		menu(void);
		void process(void);
	
	private:
	
		typedef enum:uint8_t {	menu_init_start,
								menu_init_delay,
								menu_read_parameter,
								menu_monitor_set_bus_voltage_range,
								menu_monitor_set_bus_voltage_res,
								menu_monitor_set_shunt_voltage_res,
								menu_monitor_set_shunt_gain,
								menu_monitor_set_mode,
								menu_monitor_set_calibration,
								menu_get_pd_data,
								menu_set_zero_offset,
								menu_set_parameter,
								menu_start,
								menu_process,
								menu_calibrating,
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
								icon_save,
								icon_delete,
								icon_cancel,
								icon_end 
								}	menu_icon_et;
								
		typedef enum:uint8_t {	menu_item_power,
								menu_item_profile,
								menu_item_settings,
								menu_item_calibration,
								menu_item_delete,
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
		
		typedef enum:uint8_t { 	pd_transition_start,			// initialize wait
								pd_transition_wait_transition,	// wait uintil transition is finished
								pd_transition_wait_power,		// wait until power flag is stable
								pd_transition_wait_final,		// wait before evaluation pd data
								pd_transition_ready
								} pd_transiton_et;
								
		typedef enum:uint8_t {	regulator_start,				// clear valid flag of measurement
								regulator_measure,				// wait until measurement is ready
								regulator_wait_transition_start,// wait until pps is in trasition
								regulator_wait_transition_end,	// wait until pps transition finishe
								regulator_wait_delay,			// delay before Measuring
								regulator_hold,					// no action
								regulator_off					// no regulation
								} regulator_et;
		
		typedef enum:uint8_t {	led_power_off,					// power LED off
								led_power_fix,					// power LED green
								led_power_pps,					// power LED yellow
								led_power_cv,					// power LED blue
								led_power_cc					// power LED red
								} led_power_et;
					
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

		typedef struct calibration_edit_s {	uint16_t	OutVoltage;
											uint16_t	OutCurrent;
											} calibration_edit_st;

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
											regulator_et	RegulatorStep;		// state variable
											} regulator_st;

		static const uint8_t ICON_LENGTH 	=8;		// 8 byte per icon
		static const uint8_t ICON_OFFSET 	=2;		// icon 0 & 1 used for progress bar
		static const uint8_t ICON_CNT		=8;		// max. 8 icons
		static const uint16_t MENU_IDLE_TIME=10000;	// cursor off after 10s
		static const uint16_t UPDATE_CYCLE  =667;	// update display during operation
		static const uint16_t UPDATE_CYCLE_SET=1500;	// update display during setup
		static const uint16_t CAL_LIMIT_U	=30000;	// absolute maximum 30V
		static const uint16_t CAL_LIMIT_I	=6000;	// absolute maximum 6A
		static const uint16_t PD_DELAY		=200;	// 200 ms delay after transition accomplisched
		static const uint16_t PD_REFRESH	=2500;	// first zero offset 5 sec after 0A
		static const uint16_t REG_DELAY		=1;		// wait ...ms before measuring
		static const uint16_t INIT_DELAY	=5000;	// display start message
		
		static const int16_t MIN_DELTA_U	=20;	// minimum delta for regulator
		static const int16_t MIN_DELTA_I	=10;	// minimum delta for regulator
		static const int16_t MAX_INTEGRAL_U	=3300;	// saturation value for integral U
		static const int16_t MIN_INCREMENT_U=20;	// minimum increment / decremnt on regulator voltage
		static const uint8_t FUSE_LIMIT		=3;		// max. 3 cycles at lowest voltage before power off
		static const uint8_t INCREMENT_LIMIT=3;		// after >3 successive increments increas integral step
		static const uint8_t MAX_OSC_I		=3;		// after >3 successive oscillations at MIN_INCREMENT stop regulator
		static const uint8_t CURRENT_LOCK_CYCLES=10;// wait n cycles after pps current limit before restarting regulation
		
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

			static const uint8_t LCD_BASE			=0x3f;
			static const uint8_t LCD_PWM			=9;
			
			static const uint8_t OUTPUT_SWITCH_PIN	=10;
		#else
			static const uint8_t POWER_LED_RED		=5;	
			static const uint8_t POWER_LED_GREEN	=3;
			static const uint8_t POWER_LED_BLUE		=4;
			static const bool	 POWER_LED_OFF		=true;	// pin State for LED OFF

			static const uint8_t LCD_BASE			=0x3f;
			static const uint8_t LCD_PWM			=0xff;
			static const uint8_t LCD_CAT4004_PIN	=40;
			static const uint8_t LCD_CAT4004_TYP	='A';

			static const uint8_t OUTPUT_SWITCH_PIN	=2;
		#endif
		
		static const uint8_t FUSB302_INT_PIN=7;
		static const enum PD_power_option_t PD_FALLBACK_POWER_OPTION=PD_POWER_OPTION_MAX_5V;

		static const uint8_t icon_tab[][ICON_LENGTH] PROGMEM;

		static const char pgmTextStart[] PROGMEM;	
		static const char pgmTextPower[] PROGMEM;	
		static const char pgmTextProfile[] PROGMEM;
		static const char pgmTextSettings[] PROGMEM;        
		static const char pgmTextCalibration[] PROGMEM;        
		static const char pgmTextPowerProfileFIX[] PROGMEM;
		static const char pgmTextPowerProfileBAT[] PROGMEM;
		static const char pgmTextPowerProfileVAR[] PROGMEM;
		static const char pgmTextPowerProfileAUG[] PROGMEM;
		static const char pgmTextPowerProfileNON[] PROGMEM;
		static const menu_icon_et pgmIconsPower[] PROGMEM;
		static const menu_icon_et pgmIconsProfile[] PROGMEM; 
		static const menu_icon_et pgmIconsSettings[] PROGMEM; 
		static const menu_icon_et pgmIconsCalibration[] PROGMEM; 
		static const menu_pos_st pgmMenuPower[] PROGMEM;
		static const menu_pos_st pgmMenuProfile[] PROGMEM;
		static const menu_pos_st pgmMenuSettings[] PROGMEM;
		static const menu_pos_st pgmMenuCalibration[] PROGMEM;
		static const menu_pos_st pgmMenuDelete[] PROGMEM;
				
		class PD_PPS_c PD_PPS;
		parameter 	Parameter;
		
		#ifdef ARDUINO_ARCH_AVR
			lcd			Lcd{LCD_BASE,LCD_PWM};
		#else
			lcd			Lcd{LCD_BASE,LCD_PWM,LCD_CAT4004_PIN,LCD_CAT4004_TYP};
		#endif
		
		INA219		monitor{0x40};
		
		menue_state_et 	main_state;
		menu_item_et	active_menu;
		menu_item_et	lock_item;
		menu_mode_et	menu_mode;
		pd_transiton_et	pd_transition_state;
		button_st		buttonState;
		int8_t			menu_item;
		uint16_t		lastAction;		// time of last button action
		uint16_t		lastUpdate;		// time of last button action
		uint16_t		lastRefesh;		// time of last PDPPS Refresh
		

		
		power_edit_st		power_edit;
		profile_edit_st		profile_edit;
		settings_edit_st	settings_edit;
		calibration_edit_st	calibration_edit;
		PD_power_info_t		RegulatorPower;
		regulator_st		RegulatorState;
		
		void draw_menu(const char * menuText, const menu_icon_et * iconList);
		void process_button(const menu_pos_st * menuPos);
		void next_menu_item(const menu_pos_st * menuPos);
		void set_cursor(uint8_t x, uint8_t y);
		uint8_t menu_pos_items(const menu_pos_st * menuPos);
		
		void doPower(void);
		void doProfile(void);
		void doSettings(void);
		void doCalibration(void);
		void doDelete(void);
		
		bool is_PD_ready(void);
		void setOptions(void);
		
		uint16_t getBusVoltageSet(void);
		uint16_t getBusCurrentSet(void);
		uint16_t getBusCurrentMin(void);
		uint16_t getOutVoltage(void);
		int16_t  getOutCurrent(void);
		uint16_t setPDPPS(uint16_t Voltage, uint16_t Current);
		uint16_t enableOutput(bool bEnable);
		
		void	setPowerLED(led_power_et led_mode);
		void	setOutput(bool bEnable);
		
		void savePDPPS(uint16_t Voltage, uint16_t Current);
		
		uint16_t getProfileCurrentMax(uint16_t mV);
		uint8_t getCurrentProfile(void);
		uint8_t getProfileCnt(void);
		void getProfile(uint8_t);
		void getLimits(void);
		void Regulator(void);
		bool RegulatorCC(int16_t delta);
		bool RegulatorCV(int16_t delta);
		void RegulatorSet(void);
		void enableRegulator(void);
		void resetRegulator(void);
		const char * getProfileText(void);
		
};

#endif //__menue
