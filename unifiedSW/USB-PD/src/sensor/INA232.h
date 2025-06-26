#ifndef _INA232_
#define _INA232_

#include <stdint.h>

class INA232
{
	public:
			
		typedef enum:uint8_t {	ADC_RANGE_0,	// 0	: +/-  20.48mV max  
								ADC_RANGE_1 	// 1	: +/-  81.96mV max
								} gain_et;

		typedef enum:uint8_t {	CURRENT, VOLTAGE } adc_selector_et;
		
		typedef enum:uint8_t {	MODE_POWER_DOWN,
								MODE_CURRENT_SINGLE,
								MODE_VOLTAGE_SINGLE,
								MODE_ALL_SINGLE,
								MODE_ADC_OFF,
								MODE_CURRENT_CONTINUOUS,
								MODE_VOLTAGE_CONTINUOUS,
								MODE_ALL_CONTINOUS
								} op_mode_et;
		
		typedef enum:uint8_t { 	LATCH_ENABLE					= 0,	// R/W 0=Transparent / 1= Latched
								ALERT_POLARITY					= 1,	// R/W 0=active low	/ 1= acitve high
								OVERFLOW_FLAG					= 2,	// R match overflow current / power claculation
								CONVERSION_READY_FLAG			= 3,	// R conversipn ready, new data available
								ALERT_FUNCTION_FLAG				= 4,	// R 0=Alert has been asserted by conversion ready
																		//   1=Alert has been asserted by alert condition
								MEMORY_ERROR					= 5, 	// R internal CRC or ECC error has occured
								CONVERSION_READY_ALERT_ENABLE	=10,	// R/W enables assertion of alert pin by conversion ready
								POWER_OVER_LIMIT_ALERT_ENABLE	=11,	// R/W enables assertion of alert pin by power over limit
								BUS_UNDER_LINIT_ALERT_ENABLE	=12,	// R/W enables assertion of alert pin by bus under limit
								BUS_OVER_LINIT_ALERT_ENABLE		=13,	// R/W enables assertion of alert pin by bus over limit
								SHUNT_UNDER_LINIT_ALERT_ENABLE	=14,	// R/W enables assertion of alert pin by shunt under limit
								SHUNT_OVER_LINIT_ALERT_ENABLE	=15		// R/W enables assertion of alert pin by shunt over limit
							} event_msk_et;
							
		INA232(uint8_t addr, uint8_t alert_pin=-1);
		
		bool isINA232(void);
		
		bool conversion_ready(uint16_t * pBusVoltage_mV);
		void clear_ready_flag(void);
		bool overflow(void);
		
		int16_t getCurrent(void);
		int16_t getShuntVoltage(void);
		uint16_t getPower(void);
		
		
		void setCalibration(uint16_t Cal);
		void setCalibration(uint16_t Rshunt_mR, uint16_t CurrentResolution_mA, uint16_t MaxCurrent_mA);
		uint16_t getCalibration(void);
		gain_et calcSuntGain(uint16_t Rshunt_mR, uint16_t MaxCurrent_mA);
		
		void reset(void);
		void setAdcAveraging(	uint8_t scale);	// 0-7 = 0-3 2^(2*scale) samples
												//		 4-7 2^(3+scale) samoles
		void setAdcConversionTime(adc_selector_et adc,
								uint8_t time);	// 0-7 ~2^(time+6)us
		void setShuntGain(		gain_et gain);
		void setOperationMode(	op_mode_et mode);				
		
		void setMaskFlag(event_msk_et Flag, bool bEnable);
		bool readMaskFlag(event_msk_et Flag);
		bool getMaskFlag(event_msk_et Flag);
		
		void enableConversionReadyAlertProcessing(bool bEnable);
		bool isConversionReadyAlertProcessing(void);
		
	private:
		static const uint16_t 	CFG_RST_MASK 	= 0x8000;	// Reset bit
		static const uint16_t 	CFG_ADCRNG_MASK	= 0x1000;	// ADC Range 0=20.48mV 1=81,96mV
		static const uint16_t 	CFG_AVG_MASK	= 0x0E00;	// number ov samples for averagin
		static const uint8_t 	CFG_AVG_SHIFT	= 9;			
		static const uint16_t	CFG_VBUSCT_MASK = 0x01C0;	// conversion time for Vbus
		static const uint8_t	CFG_VBUSCT_SHIFT= 6;		
		static const uint16_t	CFG_VSHCT_MASK	= 0x0038;	// converison time for Vshunt
		static const uint8_t	CFG_VSHCT_SHIFT	= 3;
		static const uint16_t	CFG_MODE_MASK	= 0x0007;	// ioerating mode
				
		static const uint8_t	REG_CONFIG			=0;
		static const uint8_t	REG_SHUNT_VOLTAGE	=1;
		static const uint8_t	REG_BUS_VOLTAGE		=2;
		static const uint8_t	REG_POWER			=3;
		static const uint8_t	REG_CURRENT			=4;
		static const uint8_t	REG_CALIBRATION		=5;
		static const uint8_t	REG_MASK			=6;
		static const uint8_t	REG_LIMIT			=7;
		static const uint8_t	REG_ID				=0x3E;
		
		static const uint32_t	MIN_SHUNT_VOLTAGE_uV=20480;
		static const uint16_t	CAL_BASE_VALUE		= 5120;
		static const uint16_t	MAX_BUS_VOLTAGE_mV	=52400;
		static const uint8_t	BUS_VOLTAGE_BITS	=15;
		static const uint8_t 	ADC_AVERAGE_MASK	=7;
		static const uint8_t	ADC_CONVERSION_MASK	=7;

		static const uint16_t	INA232_ID			=0x5449;

		bool bConversionReadyProcessing;
		bool bOverflow;
		uint8_t current_scale;
		uint8_t i2c_address;
		uint8_t alert_pin;
		uint16_t MaskRegisterValue;
		uint32_t lastBusVoltage;
		
		uint16_t read_word(uint8_t reg);
		void write_word(uint8_t reg, uint16_t val);
};

#endif // _INA232_
