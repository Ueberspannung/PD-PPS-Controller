/* ********************************************************************
 * Warpper for FUSB302 libraries ( Ryan Ma / Kai Liebich )
 * 
 * use #define PD_UFP_LOG to switch do PD_UFP_Log_c
 * 
 * adds some functionality:
 * -	get number of available PDOs
 * -	get currently active PDO
 * -	get PDO information of currently active PDO in mV / mA and 10 mW
 * -	get currently set voltage in mV
 * -	get currently set current in mA
 * -	init / set function for PPS in mV / mA
 * -	init / set function for Fixed PDOs using mV (select <= except 5V)
 * 
 */

#ifndef __PD_PPS_H__
#define __PD_PPS_H__

//#define PD_UFP_LOG

#include "PD_UFP.h"

#ifndef PD_UFP_LOG
class PD_PPS_c: public PD_UFP_c
{
	public:
		PD_PPS_c(void):fallback_power_option(PD_POWER_OPTION_MAX_5V){}
#else 
class PD_PPS_c: public PD_UFP_Log_c
{
	public:
		PD_PPS_c(pd_log_level_t log_level =  PD_LOG_LEVEL_VERBOSE):	PD_UFP_Log_c(log_level),
																fallback_power_option(PD_POWER_OPTION_MAX_5V){}
#endif	// PD_UFP_LOGLEVEL
	
		void set_int_pin(uint8_t int_pin); 									// set int pin
		void set_fallback_power_option(enum PD_power_option_t power_option);// set fallback
	
        void init_PD_PPS(uint16_t mV);										// init Fix profile 5V >= Uprofile <= mV
        void init_PD_PPS(uint16_t mV, uint16_t mA);							// init PPS profile
		
        void set_PD_PPS(uint16_t mV);										// set Fix profile 5V >= Uprofile <= mV
        bool set_PD_PPS(uint16_t mV, uint16_t mA);							// set PPS profile
	
        uint16_t get_voltage_mV(void);     									// Voltage in 50mV units, 20mV(PPS)
        uint16_t get_current_mA(void); 							    		// Current in 10mA units, 50mA(PPS)
				
		uint8_t get_PDO_cnt(void);											// get advertised number of PDOs
		uint8_t get_selected_PDO(void);										// get currently selected PDO
		bool 	get_PDO_info(uint8_t PDO, PD_power_info_t * power_info);	// gets advertised voltage in mV
																			// current in mA, power in 0,01W 
		enum PD_power_option_t calc_power_option(uint16_t mV);	

		uint16_t clock_ms(void);	
																			
	protected:
		enum PD_power_option_t fallback_power_option;
};

#endif //__PD_PPS_H__

