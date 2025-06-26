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
