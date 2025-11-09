#include "convert.h"
#include "../ASCII/ASCII_ctrl.h"

const uint32_t	convert_c::power10[]={    1ul,   
									     10ul,    100ul,    1000ul,
									  10000ul, 100000ul, 1000000ul };

convert_c::convert_c(int32_t data, uint8_t decimals)
{	// split signed to integer and decimals
    init();
	negative=data<0;
	if (negative) data=-data;
	decimalsPart=data%power10[decimals];
	integerPart=data/power10[decimals];
	this->decimals=decimals;
}	// split signed to integer and decimals

convert_c::convert_c(const char *buffer)
{	// convert string to integer and decimals
	init();
	buffer=skipSpace(buffer);
	// check sign
    if (*buffer=='-') 
    {
        negative=true;
        buffer++;
    }
	// get integer part
	while ((*buffer>='0') && (*buffer<='9'))
	{
		integerPart*=10;
		integerPart+=*buffer-'0';
		buffer++;
	}
	decimals=0;
	if (*buffer=='.')
	{	// decimals part
		buffer++;
		while ((*buffer>='0') && (*buffer<='9'))
		{
			decimalsPart*=10;
			decimalsPart+=*buffer-'0';
			buffer++;
			decimals++;
		}
	}	// decimals part
}	// convert string to integer and decimals

char* convert_c::getStringUnsigned(char *buffer, uint8_t size, uint8_t decimals, bool null)
{	// unsigend val
	
	if (null) buffer[size]=NUL;
	if (decimals>0)
	{
		if (decimals<this->decimals) 
			decimalsPart/=power10[this->decimals-decimals];
		else
			decimalsPart*=power10[decimals-this->decimals];
		buffer[size-decimals-1]='.';
		getString(&buffer[size-decimals],decimals,true);
		size=size-decimals-1;
	}
	getString(buffer,size,false);
	return buffer;
}	// unsigend val

char* convert_c::getStringUnsignedZeros(char *buffer, uint8_t size, uint8_t decimals, bool null)
{	//	getStringUnsignedZeros
	getStringUnsigned(buffer,size,decimals,null);
	for (uint8_t n=0; n<size;n++) if (buffer[n]==SPACE) buffer[n]='0';
	return buffer;
}	//	getStringUnsignedZeros


char* convert_c::getStringUnsigned(char *buffer, uint8_t size, uint8_t decimals, const char * unit)
{
	char * temp=&buffer[size];
	
	while ((*temp++=*unit++));
	
	return getStringUnsigned(buffer,size,decimals,false);
}

char* convert_c::getStringSigned(char *buffer, uint8_t size, uint8_t decimals, bool null)
{	// signed val
	buffer[0]=SPACE;
	getStringUnsigned(&buffer[1],size-1,decimals,null);
	decimals=0;
	while (buffer[decimals]==SPACE) decimals++;
	if (decimals>0) decimals--;
	if (negative) buffer[decimals]='-';
	return buffer;
}	// signed val

char* convert_c::getStringSigned(char *buffer, uint8_t size, uint8_t decimals, const char * unit)
{
	char * temp=&buffer[size];
	
	while ((*temp++=*unit++));
	
	return getStringSigned(buffer,size,decimals,false);
}

char* convert_c::getStringTime(char* buffer, bool bDays, uint8_t decimals)
{	// getStringTime
	/* 	get fomatted time 
	 * "dd hh:mm:ss.xxx"
	 * select dd by setting days to true
	 */
	uint8_t days;
	uint16_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint32_t temp=integerPart;
	uint8_t bufferPos;
	
	seconds=temp%60; temp/=60;
	minutes=temp%60; temp/=60;
	if (bDays)
	{ // calc days and hours
		hours=temp%24; temp/=24;
		days=temp;
	} // calc days and hours
	else
	{	// calc hours only
		hours=temp;
		days=0;
	}	// calc hours only
	
	bufferPos=0;
	if (bDays)
	{	// convert using days (max 49) and hours
		convert_c(days,0).getStringUnsigned(buffer,2,0);
		bufferPos+=2;
		buffer[bufferPos++]=SPACE;
		convert_c(hours,0).getStringUnsigned(&buffer[bufferPos],2,0);
		bufferPos+=2;
	}	// convert using days and hours
	else
	{	// use hours only (max 1193)
		convert_c(hours,0).getStringUnsigned(buffer,4,0);
		bufferPos+=2;
	}	// use hours only
	buffer[bufferPos++]=':';
	convert_c(minutes,0).getStringUnsignedZeros(&buffer[bufferPos],2,0);
	bufferPos+=2;
	buffer[bufferPos++]=':';
	temp=integerPart;
	integerPart=seconds;
	getStringUnsignedZeros(&buffer[bufferPos],(decimals)?(decimals+3):(2),decimals);
	integerPart=temp;
	return buffer;
}	// getStringTime


uint32_t convert_c::getUnsigned(uint8_t decimals)
{
    if (decimals>this->decimals)
    	return (integerPart*power10[this->decimals]+decimalsPart)*power10[decimals-this->decimals];
    else
    	return (integerPart*power10[this->decimals]+decimalsPart)/power10[this->decimals-decimals];
}

int32_t convert_c::getSigned(uint8_t decimals)
{
    if (negative)
        return -getUnsigned(decimals);
    else
        return getUnsigned(decimals);
}



void convert_c::init(void)
{	// init
	integerPart=0;
	decimalsPart=0;
	decimals=0;
	negative=false;
}	// init


const char* convert_c::skipSpace(const char* buffer)
{	// skipSpace
	while (*buffer==SPACE) buffer++;
	return buffer;	
}	// skipSpace

void convert_c::getString(char *buffer,uint8_t size,bool decimals)
{	// getString
	uint32_t val;
	bool	 nullFlag;
	
	if (decimals) 
		val=decimalsPart;
	else
		val=integerPart;
	
	nullFlag=val==0;
	
	while (size-->0)
	{
		buffer[size]='0'+val%10;
		if ((!decimals) && (!nullFlag) && (val==0))
			buffer[size]=SPACE;
		val/=10;
		nullFlag=false;
	}
}	// getString
