/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_ARDUINO_ZERO_
#define _VARIANT_ARDUINO_ZERO_

// The definitions here needs a SAMD core >=1.6.10
#define ARDUINO_SAMD_VARIANT_COMPLIANCE 10610

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC		(32768ul)

/** Master clock frequency */
#define VARIANT_MCK			  (48000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
#include "SERCOM.h"
#include "Uart.h"
#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#ifdef __cplusplus
extern "C" unsigned int PINCOUNT_fn();
#endif
#define PINS_COUNT           (PINCOUNT_fn())
#define NUM_DIGITAL_PINS     (20u)
#define NUM_ANALOG_INPUTS    (6u)
#define NUM_ANALOG_OUTPUTS   (1u)
#define analogInputToDigitalPin(p)  ((p < 6u) ? (p) + 14u : -1)

#define digitalPinToPort(P)        ( &(PORT->Group[g_APinDescription[P].ulPort]) )
#define digitalPinToBitMask(P)     ( 1 << g_APinDescription[P].ulPin )
//#define analogInPinToBit(P)        ( )
#define portOutputRegister(port)   ( &(port->OUT.reg) )
#define portInputRegister(port)    ( &(port->IN.reg) )
#define portModeRegister(port)     ( &(port->DIR.reg) )
#define digitalPinHasPWM(P)        ( g_APinDescription[P].ulPWMChannel != NOT_ON_PWM || g_APinDescription[P].ulTCChannel != NOT_ON_TIMER )

/*
 * digitalPinToTimer(..) is AVR-specific and is not defined for SAMD
 * architecture. If you need to check if a pin supports PWM you must
 * use digitalPinHasPWM(..).
 *
 * https://github.com/arduino/Arduino/issues/1833
 */
// #define digitalPinToTimer(P)

// LEDs
#define PIN_LED_13           (13u)
// remove pin definitions in oder to use as gpio
// modify variants.cpp at pin 25/26 in order to use interrupts
// pin 25 is PB03 EXTINT[3]
// pin 26 is PA27 EXTINT[15]
//#define PIN_LED_RXL          (25u)
//#define PIN_LED_TXL          (26u)
#define PIN_LED              PIN_LED_13
#define PIN_LED2             PIN_LED_RXL
#define PIN_LED3             PIN_LED_TXL
#define LED_BUILTIN          PIN_LED_13

/*
 * Analog pins
 */
#define PIN_A0               (14ul)
#define PIN_A1               (15ul)
#define PIN_A2               (16ul)
#define PIN_A3               (17ul)
#define PIN_A4               (18ul)
#define PIN_A5               (19ul)
#define PIN_DAC0             (14ul)

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t DAC0 = PIN_DAC0;
#define ADC_RESOLUTION		12

// Other pins
#define PIN_ATN              (38ul)
static const uint8_t ATN = PIN_ATN;

/*
 * Serial interfaces
 */
// Serial (EDBG)
/*
#define PIN_SERIAL_RX       (31ul)
#define PIN_SERIAL_TX       (30ul)
#define PAD_SERIAL_TX       (UART_TX_PAD_2)
#define PAD_SERIAL_RX       (SERCOM_RX_PAD_3)
*/
// Serial1
#define PIN_SERIAL1_RX       (0ul)
#define PIN_SERIAL1_TX       (1ul)
#define PAD_SERIAL1_TX       (UART_TX_PAD_2)
#define PAD_SERIAL1_RX       (SERCOM_RX_PAD_3)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 2
/* *****************************************************
 * original SPI defs for Arduino Zero on 6pin SPI Header
#define PIN_SPI_MISO		(22u)
#define PIN_SPI_MOSI		(23u)
#define PIN_SPI_SCK			(24u)
#define PERIPH_SPI			sercom4
#define PAD_SPI_TX			SPI_PAD_2_SCK_3
#define PAD_SPI_RX			SERCOM_RX_PAD_0

static const uint8_t SS		= PIN_A2 ;	// SERCOM4 last PAD is present on A2 but HW SS isn't used. Set here only for reference.
static const uint8_t MOSI	= PIN_SPI_MOSI ;
static const uint8_t MISO	= PIN_SPI_MISO ;
static const uint8_t SCK	= PIN_SPI_SCK ;
*/

// Mapping sercom1 to SPI-Pins D10-13
#define PIN_SPI_MISO		(12u)
#define PIN_SPI_MOSI		(11u)
#define PIN_SPI_SCK			(13u)
#define PIN_SPI_SS			(10u)
#define PERIPH_SPI			sercom1
#define PAD_SPI_TX			SPI_PAD_0_SCK_1
#define PAD_SPI_RX			SERCOM_RX_PAD_3

static const uint8_t SS		= PIN_SPI_SS ;	//  HW SS isn't used. Set here only for reference.
static const uint8_t MOSI	= PIN_SPI_MOSI ;
static const uint8_t MISO	= PIN_SPI_MISO ;
static const uint8_t SCK	= PIN_SPI_SCK ;



// SPI1: Connected to SD	
//		SDCARD_CD	  (26u)	// PA27, MKR Zero:	(30u)
#define PIN_SPI1_MISO (5u)	// PA15, MKR Zero:	(29u)
#define PIN_SPI1_MOSI (22u)	// PA12, MKR Zero:	(26u)
#define PIN_SPI1_SCK  (38u)	// PA13, MKR Zero:	(27u)
#define PIN_SPI1_SS   (2u)	// PA14, MKR Zero:	(28u)
#define PERIPH_SPI1   sercom4
#define PAD_SPI1_TX   SPI_PAD_0_SCK_1
#define PAD_SPI1_RX   SERCOM_RX_PAD_3
static const uint8_t SS1   = PIN_SPI1_SS;
static const uint8_t MOSI1 = PIN_SPI1_MOSI;
static const uint8_t MISO1 = PIN_SPI1_MISO;
static const uint8_t SCK1  = PIN_SPI1_SCK;
/* ******************
 * to be modified in variants.cpp:
  { PORTA, 12, PIO_SERCOM_ALT, (PIN_ATTR_NONE), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MOSI: SERCOM4/PAD[0]
  { PORTA, 13, PIO_SERCOM_ALT, (PIN_ATTR_NONE), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SCK:  SERCOM4/PAD[1]
  { PORTA, 14, PIO_DIGITAL,    (PIN_ATTR_NONE), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SS:   as GPIO
  { PORTA, 15, PIO_SERCOM_ALT, (PIN_ATTR_NONE), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MISO: SERCOM4/PAD[3]
  { PORTA, 27, PIO_DIGITAL,    (PIN_ATTR_NONE), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
*/

// Needed for SD library
#define SDCARD_SPI      SPI1
#define SDCARD_MISO_PIN PIN_SPI1_MISO
#define SDCARD_MOSI_PIN PIN_SPI1_MOSI
#define SDCARD_SCK_PIN  PIN_SPI1_SCK
#define SDCARD_SS_PIN   PIN_SPI1_SS


/* ******************************************
 * for SPI1 to SPI 5
 * fill in the necessary definitions pins, pads, sercoms


static const uint8_t SS1	= PIN_A2 ;	
static const uint8_t MOSI1 	= PIN_SPI1_MOSI ;
static const uint8_t MISO1 	= PIN_SPI1_MISO ;
static const uint8_t SCK1  	= PIN_SPI1_SCK ;


#define PERIPH_SPI2
#define PIN_SPI2_MISO
#define PIN_SPI2_SCK
#define PIN_SPI2_MOSI
#define PAD_SPI2_TX
#define PAD_SPI2_RX

static const uint8_t SS2	= PIN_A2 ;	
static const uint8_t MOSI2 	= PIN_SPI2_MOSI ;
static const uint8_t MISO2 	= PIN_SPI2_MISO ;
static const uint8_t SCK2  	= PIN_SPI2_SCK ;


#define PERIPH_SPI3
#define PIN_SPI3_MISO
#define PIN_SPI3_SCK
#define PIN_SPI3_MOSI
#define PAD_SPI3_TX
#define PAD_SPI3_RX

static const uint8_t SS3	= PIN_A2 ;	
static const uint8_t MOSI3 	= PIN_SPI3_MOSI ;
static const uint8_t MISO3 	= PIN_SPI3_MISO ;
static const uint8_t SCK3  	= PIN_SPI3_SCK ;


#define PERIPH_SPI4
#define PIN_SPI4_MISO
#define PIN_SPI4_SCK
#define PIN_SPI4_MOSI
#define PAD_SPI4_TX
#define PAD_SPI4_RX

static const uint8_t SS4	= PIN_A2 ;	
static const uint8_t MOSI4 	= PIN_SPI4_MOSI ;
static const uint8_t MISO4 	= PIN_SPI4_MISO ;
static const uint8_t SCK4  	= PIN_SPI4_SCK ;


#define PERIPH_SPI5
#define PIN_SPI5_MISO
#define PIN_SPI5_SCK
#define PIN_SPI5_MOSI
#define PAD_SPI5_TX
#define PAD_SPI5_RX

static const uint8_t SS5	= PIN_A2 ;	
static const uint8_t MOSI5 	= PIN_SPI5_MOSI ;
static const uint8_t MISO5 	= PIN_SPI5_MISO ;
static const uint8_t SCK5  	= PIN_SPI5_SCK ;

*/


/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (20u)
#define PIN_WIRE_SCL         (21u)
#define PERIPH_WIRE          sercom3
#define WIRE_IT_HANDLER      SERCOM3_Handler

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

/*
 * USB
 */
#define PIN_USB_HOST_ENABLE (27ul)
#define PIN_USB_DM          (28ul)
#define PIN_USB_DP          (29ul)

/*
 * I2S Interfaces
 */
#define I2S_INTERFACES_COUNT 1

#define I2S_DEVICE          0
#define I2S_CLOCK_GENERATOR 3
#define PIN_I2S_SD          (9u)
#define PIN_I2S_SCK         (1u)
#define PIN_I2S_FS          (0u)

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

/*	=========================
 *	===== SERCOM DEFINITION
 *	=========================
*/
extern SERCOM sercom0;
extern SERCOM sercom1;
extern SERCOM sercom2;
extern SERCOM sercom3;
extern SERCOM sercom4;
extern SERCOM sercom5;

//replac by alias
//extern Uart Serial;
extern Uart Serial1;

#endif

#ifdef __cplusplus
extern "C" {
#endif
unsigned int PINCOUNT_fn();
#ifdef __cplusplus
}
#endif

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_USBVIRTUAL      SerialUSB
#define SERIAL_PORT_MONITOR         Serial
// Serial has no physical pins broken out, so it's not listed as HARDWARE port
#define SERIAL_PORT_HARDWARE        Serial1
#define SERIAL_PORT_HARDWARE_OPEN   Serial1


// Alias Serial to SerialUSB
#define Serial                      SerialUSB



#endif /* _VARIANT_ARDUINO_ZERO_ */

