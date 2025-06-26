#include "SerialClass.h"

#ifdef HAS_USB_SERIAL
SerialClass::SerialClass(Serial_ & port)
{
	USBport=&port;
	HWport=NULL;
	init();
}

#endif


SerialClass::SerialClass(HardwareSerial & port)
{
	HWport=&port;
	#ifdef HAS_USB_SERIAL
	USBport=NULL;
	#endif
	init();
}

void SerialClass::init(void)
{
}

SerialClass::operator bool()
{
	bool bActive=HWport;

	#ifdef HAS_USB_SERIAL
	if (!bActive)
	{
		/*
		 * Windows: SerialUSB becomes true when connection is established
		 *          but stays true when USB connection is closed
		 * 			dtr() becomes false when USB connection is closed
		 * Linux:   SerialUSB represents the connection state of an
		 *          USB serial port. no issues
		 * 
		 * Issue:	A cakk to USBPort costs 10 ms , use dtr() instead
		 */ 
		bActive=*USBport;
		if (bActive)
			bActive=USBport->dtr();
	}
	#endif
	return bActive;
}

void SerialClass::begin(unsigned long baud, uint16_t config)
{
	if (HWport)
	{	// HardwareSerial
		HWport->begin(baud,config);
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		USBport->begin(baud,config);
	}	// CDC-Serial
	#endif
}

void SerialClass::end()
{
	if (HWport)
	{	// HardwareSerial
		HWport->end();
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		USBport->end();
	}	// CDC-Serial
	#endif
}

int	SerialClass::available(void)
{
	int cnt=0;
	if (HWport)
	{	// HardwareSerial
		cnt=HWport->available();
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		cnt=USBport->available();
	}	// CDC-Serial
	#endif
	return cnt;
}

int SerialClass::peek(void)
{
	int next_char=-1;
	if (HWport)
	{	// HardwareSerial
		next_char=HWport->peek();
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		next_char=USBport->peek();
	}	// CDC-Serial
	#endif
	return next_char;
}

int SerialClass::read(void)
{
	int next_char=-1;
	if (HWport)
	{	// HardwareSerial
		next_char=HWport->read();
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		next_char=USBport->read();
	}	// CDC-Serial
	#endif
	return next_char;
}

int SerialClass::availableForWrite(void)
{
	int cnt=0;
	if (HWport)
	{	// HardwareSerial
		cnt=HWport->availableForWrite();
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		cnt=USBport->availableForWrite();
	}	// CDC-Serial
	#endif
	return cnt;
}

void SerialClass::flush(void)
{
	if (HWport)
	{	// HardwareSerial
		HWport->flush();
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		USBport->flush();
	}	// CDC-Serial
	#endif
}

size_t SerialClass::write(uint8_t c)
{
	int cnt=0;
	if (HWport)
	{	// HardwareSerial
		cnt=HWport->write(c);
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		cnt=USBport->write(c);
	}	// CDC-Serial
	#endif
	return cnt;
}

size_t SerialClass::write(const uint8_t *buffer, size_t size)
{
	int cnt=0;
	if (HWport)
	{	// HardwareSerial
		cnt=HWport->write(buffer,size);
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		cnt=USBport->write(buffer,size);
	}	// CDC-Serial
	#endif
	return cnt;
}

size_t SerialClass::readBytes( char *buffer, size_t length)
{
	int cnt=0;
	if (HWport)
	{	// HardwareSerial
		cnt=HWport->readBytes(buffer,length);
	}	// HardwareSerial
	#ifdef HAS_USB_SERIAL
	else
	{	// CDC-Serial
		cnt=USBport->readBytes(buffer,length);
	}	// CDC-Serial
	#endif
	return cnt;
}

int32_t SerialClass::readBreak()
{
	int32_t time=0;
	#ifdef HAS_USB_SERIAL
	if (!HWport)
	{	// CDC-Serial
		time=USBport->readBreak();
	}	// CDC-Serial
	#endif
	return time;
}

bool SerialClass::dtr(void)
{
	bool bDTR=true;
	#ifdef HAS_USB_SERIAL
	if (!HWport)
	{	// CDC-Serial
		bDTR=USBport->dtr();
	}	// CDC-Serial
	#endif
	return bDTR;
}

bool SerialClass::rts(void)
{
	bool bRTS=true;
	#ifdef HAS_USB_SERIAL
	if (!HWport)
	{	// CDC-Serial
		bRTS=USBport->rts();
	}	// CDC-Serial
	#endif
	return bRTS;
}
