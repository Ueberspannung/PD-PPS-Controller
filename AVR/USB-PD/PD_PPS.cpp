#include "PD_PPS.h"

/* *********************************************************************
 *                       public functions
 */									

void PD_PPS_c::set_int_pin(uint8_t int_pin) 									// set int pin
{	// set_int_pin
	this->int_pin=int_pin;
}	// set_int_pin

void PD_PPS_c::set_fallback_power_option(enum PD_power_option_t power_option)	// set fallback
{	// set_fallback_power_option
	fallback_power_option=power_option;
}	// set_fallback_power_option

void PD_PPS_c::init_PD_PPS(uint16_t mV)											// init Fix profile 5V >= Uprofile <= mV
{	// init_PD_PPS (fix)
	init_PD_PPS(mV,0);
}	// init_PD_PPS (fix)

void PD_PPS_c::init_PD_PPS(uint16_t mV, uint16_t mA)							// init PPS profile
{	// init_PD_PPS (fix / pps)
	if (mA==0)
	{	// init Fix profile
		init(int_pin,calc_power_option(mV));
	}	// init Fix profile
	else
	{	// init PPS profile
		init_PPS(int_pin,mV/20,mA/50,fallback_power_option);
	}	// init PPS profile
}	// init_PD_PPS (fix / pps)
	
void PD_PPS_c::set_PD_PPS(uint16_t mV)										// set Fix profile 5V >= Uprofile <= mV
{	// set_PD_PPS (fix)
	set_power_option(calc_power_option(mV));
}	// set_PD_PPS (fix)

bool PD_PPS_c::set_PD_PPS(uint16_t mV, uint16_t mA)							// set PPS profile
{	// set_PD_PPS (pps)
	return set_PPS(mV/20,mA/50);
}	// set_PD_PPS (pps)

uint16_t PD_PPS_c::get_voltage_mV(void)
{	// get_voltage_mV
	if (status_power!=STATUS_POWER_PPS)
		return ready_voltage*50;	// Fix 50 mV units
	else 
		return ready_voltage*20;	// PPS 20 mV units
} 	// get_voltage_mV

uint16_t PD_PPS_c::get_current_mA(void)
{	// get_current_mA
		if (status_power!=STATUS_POWER_PPS)
			return ready_current*10; 	// Fix 10 mA units
		else 
			return	ready_current*50;	// PPS 50 mA units
}	// get_current_mA

uint8_t PD_PPS_c::get_PDO_cnt(void)	// get advertised number of PDOs
{	// get_PDO_cnt
	return protocol.power_data_obj_count;
}	// get_PDO_cnt

uint8_t PD_PPS_c::get_selected_PDO(void) // get currently selected PDO
{	// get_selected_PDO
	return protocol.power_data_obj_selected;
}	// get_selected_PDO
										
bool 	PD_PPS_c::get_PDO_info(uint8_t PDO, PD_power_info_t * power_info)	// gets advertised voltage in mV
{ 	//	get_PDO_info 														   current in mA, power in 0,01W
	memset(power_info,0,sizeof(PD_power_info_t));
	if (PD_protocol_get_power_info(&protocol,PDO,power_info))
	{	// recalculate units
		power_info->min_v*=50;	//  50 mV units => [min_v]= 1 mV
		power_info->max_v*=50;	//  50 mV units => [max_v]= 1 mV
		power_info->max_i*=10;	//  10 mA units => [max_i]= 1 mA
		power_info->max_p*=25;	// 250 mW units => [max_p]=10 mW
		return true;
	}	// recalculate units
	else
		return false;
}	//	get_PDO_info																			

																			 
enum PD_power_option_t PD_PPS_c::calc_power_option(uint16_t mV)
{	// calc_power_option
	enum PD_power_option_t Option=PD_POWER_OPTION_MAX_5V; 
	if (mV<9000) Option=PD_POWER_OPTION_MAX_5V;
	else if (mV<12000) Option=PD_POWER_OPTION_MAX_9V;
	else if (mV<15000) Option=PD_POWER_OPTION_MAX_12V;
	else if (mV<20000) Option=PD_POWER_OPTION_MAX_15V;
	else if (mV==20000) Option=PD_POWER_OPTION_MAX_20V;
	else Option=PD_POWER_OPTION_MAX_VOLTAGE;
	return Option;
}	// calc_power_option


uint16_t PD_PPS_c::clock_ms(void)
{	// get PD_UFP_c clock
	return PD_UFP_c::clock_ms();
}	// get PD_UFP_c clock
