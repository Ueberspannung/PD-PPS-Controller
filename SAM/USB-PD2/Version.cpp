#include "Version.h"

const char PROGMEM Version_c::VersionString[] PROGMEM 	= "4.0";
const char PROGMEM Version_c::DateString[] PROGMEM 		= __DATE__;

const char * Version_c::pgmGetVersion(void)
{
	return VersionString;
}

const char * Version_c::pgmGetBuiltDate(void)
{
	return DateString;
}
