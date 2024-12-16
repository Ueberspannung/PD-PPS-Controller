//#include <Arduino.h>
#ifndef	_eeprom_h_
#define _eeprom_h_

#include <stdint.h>
#include <stdbool.h>

/* 	set chip desinty, i2c Base address, number of succesive chips
	currently supports max. 64kByte max. Addresspace
*/
class eeprom
{
	public:
		typedef enum:uint8_t {	eeprom_1kbit, eeprom_2kbit, 
								eeprom_4kbit, eeprom_8kbit, 
								eeprom_16kbit, eeprom_32kbit, 
								eeprom_64kbit, eeprom_128kbit,
								eeprom_256kbit, eeprom_512kbit } eeprom_density_t;
								
		eeprom(eeprom_density_t density_kbit, uint8_t i2c_base, uint8_t devices=1);

		bool test(	void);				// test if all chips are available
		bool test(	uint8_t device);	// test if a specific chip (0-7) ist available
		bool read(	uint16_t address, 	// read byte (array)
					uint8_t * pData, 
					uint16_t nCnt=1);
		bool read(	uint16_t address, 	// read word
					uint16_t * pData);
		bool write(	uint16_t address, 	// write byte
					uint8_t Data);		
		bool write(	uint16_t address, 	// write word
					uint16_t Data);
		bool write(	uint16_t address, 	// write byte array
					uint8_t * pData, 
					uint16_t nCnt=1);
					
		bool write(	uint16_t address, 	// write byte array
					const __FlashStringHelper * pData, 
					uint16_t nCnt=1);
					
		bool write_P(	uint16_t address, 	// write byte array
					const char * pData, 
					uint16_t nCnt=1);
					
		uint16_t getAddressSpace(void);
		
		

	private:

		#ifdef ARDUINO_ARCH_SAMD
		static const uint8_t BUFFER_LENGTH PROGMEM = 16;
		#endif
	
	
		uint8_t		chip_address;		// i2c-address of first chip with succesive adresses
		uint8_t		chip_cnt;			// number of memories with succesive adresses
		uint8_t 	i2c_addressbytes;	// calculated from eeprom density
		uint8_t		chip_address_bits;	// number of addressbits within a singele chip
		uint16_t	writeDelayMax;		// max duration of write cycle >5ms
		uint8_t		chip_page_size;		// internal write buffer, or half Wire buffer, whatever is less
		uint8_t		chip_offset_shift;	// in case of 4 / 8 / 16 kbit, addressincrement needs a shift
										// because internally organizes as 2/4/8 2kbit chips
		eeprom_density_t density;
		
		bool i2cProbe(	uint8_t i2c_address);	// test if chip at i2c_address is available
		bool read_chip(	uint8_t i2c_address, 	// read a sequence of bytes, max 1/2 Wire Buffer
						uint16_t address, 
						uint8_t * pData, 
						uint16_t nCnt=1);
		bool write_byte(uint8_t i2c_address, 	// write a singele byte
						uint16_t address, 
						uint8_t Data);
		bool write_page(uint8_t i2c_address, 	// write a page page size max 1/2 Wire Buffer
						uint16_t address, 
						uint8_t *pData, 
						uint16_t nCnt=1);
		bool write_page_P(uint8_t i2c_address, 	// write a page page size max 1/2 Wire Buffer
						uint16_t address, 
						const uint8_t *pData, 
						uint16_t nCnt=1);
		bool acknowledgePolling(uint8_t i2c_address);	// acknowledge polling after write

		uint8_t getChipAddress(uint16_t address);		// calculate i2c_address from sequential memory address
		uint16_t getAddressOffset(uint16_t address);	// calculate chip internal address offset
};

#endif // _eeprom_h_
