#include "Version.h"

const char Version_c::VersionString[]  	= "5.2";
const char Version_c::DateString[] 		= __DATE__;

const char * Version_c::pgmGetVersion(void)
{
	return VersionString;
}

const char * Version_c::pgmGetBuiltDate(void)
{
	return DateString;
}
