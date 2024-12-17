#ifndef _INA219_
#define _INA219_

#include <stdint.h>

class INA219
{
	public:
	
		typedef enum:uint8_t {	BUS_VOLTAGE_16V,
								BUS_VOLTAGE_32V 
								} bus_voltage_range_et;
		
		typedef enum:uint8_t {	PG_1,		// 1	: +/-  40mV max  
								PG_div_2, 	// 1/2	: +/-  80mV max
								PG_div_4, 	// 1/4	: +/- 160mV max
								PG_div_8	// 1/8	: +/- 320mV max
								} gain_et;

		typedef enum:uint8_t {	CURRENT, VOLTAGE } adc_selector_et;

		typedef enum:uint8_t {	ADC_RES_9BIT,
								ADC_RES_10BIT,
								ADC_RES_11BIT,
								ADC_RES_12BIT
								} adc_res_et;
		
		typedef enum:uint8_t {	MODE_POWER_DOWN,
								MODE_CURRENT_SINGLE,
								MODE_VOLTAGE_SINGLE,
								MODE_ALL_SINGLE,
								MODE_ADC_OFF,
								MODE_CURRENT_CONTINUOUS,
								MODE_VOLTAGE_CONTINUOUS,
								MODE_ALL_CONTINOUS
								} op_mode_et;
		
	
		INA219(uint8_t addr);
		
		bool conversion_ready(uint16_t * pBusVoltage_mV);
		void clear_ready_flag(void);
		bool overflow(void);
		
		int16_t getCurrent(void);
		int16_t getShuntVoltage(void);
		uint16_t getPower(void);
		
		void setCalibration(uint16_t Cal);
		uint16_t getCalibration(void);
		
		void reset(void);
		void setBusVoltageRange(bus_voltage_range_et range);
		void setAdcResolution(	adc_selector_et adc,
								adc_res_et resolution);
		void setAdcAveraging(	adc_selector_et adc,
								uint8_t scale);	// 0-7 = averaging 2^scale samples
		void setShuntGain(		gain_et gain);
		void setOperationMode(	op_mode_et mode);				
		
	private:
		static const uint16_t 	CFG_RST_MASK 	= 0x8000;
		static const uint16_t 	CFG_BRNG_MASK	= 0x2000;
		static const uint16_t 	CFG_PG_MASK		= 0x1800;
		static const uint8_t 	CFG_PG_SHIFT	= 11;
		static const uint16_t	CFG_BADC_MASK	= 0x0780;
		static const uint8_t	CFG_BADC_SHIFT	= 7;
		static const uint16_t	CFG_SADC_MASK	= 0x0078;
		static const uint8_t	CFG_SADC_SHIFT	= 3;
		static const uint16_t	CFG_MODE_MASK	= 0x0007;
		
		static const uint8_t	REG_CONFIG			=0;
		static const uint8_t	REG_SHUNT_VOLTAGE	=1;
		static const uint8_t	REG_BUS_VOLTAGE		=2;
		static const uint8_t	REG_POWER			=3;
		static const uint8_t	REG_CURRENT			=4;
		static const uint8_t	REG_CALIBRATION		=5;

		static const uint8_t	ADC_AVERAGE_FLAG 	=0x08;
		static const uint8_t	ADC_AVERAGE_MASK 	=0x07;
		

		bool bOverflow;
		uint8_t i2c_address;
		
		uint16_t read_word(uint8_t reg);
		void write_word(uint8_t reg, uint16_t val);
};

#endif // _INA219_
