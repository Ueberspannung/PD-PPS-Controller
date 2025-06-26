#ifndef __Version_h__
#define __Version_h__

#include <Arduino.h>
class Version_c
{
	public:
		const char * pgmGetVersion(void);
		const char * pgmGetBuiltDate(void);
	
	private:
		static const char VersionString[] PROGMEM;
		static const char DateString[] PROGMEM;
};

#endif
