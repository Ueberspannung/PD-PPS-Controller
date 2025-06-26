#ifndef __convert_h__
#define __convert_h__
#include <stdint.h>

class convert_c
{
	public:
		//convert_c(uint32_t data, uint8_t decimals);
		convert_c(int32_t data, uint8_t decimals);
		convert_c(const char *buffer);
		
		// buffer needs to 1 char bigger than size (trailing '\0')
		char* getStringUnsigned(char *buffer, uint8_t size, uint8_t decimals, bool null=true);
		char* getStringUnsigned(char *buffer, uint8_t size, uint8_t decimals, const char * unit);
		char* getStringSigned(char *buffer, uint8_t size, uint8_t decimals, bool null=true);
		char* getStringSigned(char *buffer, uint8_t size, uint8_t decimals, const char * unit);
		
		uint32_t getUnsigned(uint8_t decimals);
		int32_t getSigned(uint8_t decimals);

		uint16_t integerPart;
		uint16_t decimalsPart;
		uint8_t	 decimals;

	private:
		static const uint32_t	power10[];
		bool	 negative;
		
		void init(void);
		const char* skipSpace(const char* buffer);
		void getString(char *buffer,uint8_t size,bool decimals);
};

#endif // __convert_h__
