#ifndef __HWTEXT_H__
#define __HWTEXT_H__
#include "config.h"

#pragma message  ( "**** Built for HW Version:" )


#if defined(PD_PPS_CONTROLLER)
	#pragma message (	"*** first HW Version ***" )

#elif defined(PD_PPS_CONTROLLER_2)
	#pragma message (	"*** second HW Version + HD44780 ***" )

#elif defined(PD_PPS_CONTROLLER_MINI)
	#pragma message (	"*** mini HW Version  ***" )
	
#else
	#pragma message ( **** no hardware defined *** )
#endif  

#if defined(PD_PPS_CONTROLLER) && defined(PD_PPS_CONTROLLER_2) ||\
	defined(PD_PPS_CONTROLLER_2) && defined(PD_PPS_CONTROLLER_MINI) ||\
	defined(PD_PPS_CONTROLLER) && defined(PD_PPS_CONTROLLER_MINI) 

	#pragma message ( ****multiple hardware defined *** )

#endif


#endif // __HWTEXT_H__

