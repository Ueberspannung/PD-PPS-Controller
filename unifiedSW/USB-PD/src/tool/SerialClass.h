#ifndef __SerialClass_h__
#define __SerialClass_h__

#include <Arduino.h>
#include <stdint.h>
#include <Stream.h>

#if defined(HAVE_CDCSERIAL) || !defined(ARDUINO_ARCH_AVR)
#define HAS_USB_SERIAL
#endif


class SerialClass: public Stream
{
	public:

		SerialClass(HardwareSerial & port);
		#ifdef HAS_USB_SERIAL
		SerialClass(Serial_ & port);
		#endif

		bool isUSB(void) { return !HWport; }

		/*
		 * puplic members of HardwareSerial
		 * 
		 */
		void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
		void begin(unsigned long baud, uint16_t config);
		void end();
 		virtual int available(void);
		virtual int peek(void);
		virtual int read(void);
		virtual int availableForWrite(void);
		virtual void flush(void);
		virtual size_t write(uint8_t);
		inline size_t write(unsigned long n) { return write((uint8_t)n); }
		inline size_t write(long n) { return write((uint8_t)n); }
		inline size_t write(unsigned int n) { return write((uint8_t)n); }
		inline size_t write(int n) { return write((uint8_t)n); }

		/*
		 * puplic members of Serial not implemented by HardwareSerial
		 * 
		 */

		virtual size_t write(const uint8_t *buffer, size_t size);
		using Print::write; // pull in write(str) from Print
		
		operator bool();
	
		size_t readBytes(char *buffer, size_t length);
		size_t readBytes( uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }
	
		// This method allows processing "SEND_BREAK" requests sent by
		// the USB host. Those requests indicate that the host wants to
		// send a BREAK signal and are accompanied by a single uint16_t
		// value, specifying the duration of the break. The value 0
		// means to end any current break, while the value 0xffff means
		// to start an indefinite break.
		// readBreak() will return the value of the most recent break
		// request, but will return it at most once, returning -1 when
		// readBreak() is called again (until another break request is
		// received, which is again returned once).
		// This also mean that if two break requests are received
		// without readBreak() being called in between, the value of the
		// first request is lost.
		// Note that the value returned is a long, so it can return
		// 0-0xffff as well as -1.
		int32_t readBreak();
	
		/*
		 * virtual declaration to mak it possible to implement a new class
		 * which implements dtr and rts pins for HardwareSerial
		 * 
		 */
		virtual bool dtr();
		virtual bool rts();

		enum {
			ONE_STOP_BIT = 0,
			ONE_AND_HALF_STOP_BIT = 1,
			TWO_STOP_BITS = 2,
		};
		enum {
			NO_PARITY = 0,
			ODD_PARITY = 1,
			EVEN_PARITY = 2,
			MARK_PARITY = 3,
			SPACE_PARITY = 4,
		};
		
	


		
	private:
		HardwareSerial 	* HWport;
		#ifdef HAS_USB_SERIAL
		Serial_			* USBport;
		#endif
		
		void init(void);
};

#endif // __SerialClass_h__
