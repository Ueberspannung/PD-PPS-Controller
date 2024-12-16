#ifndef __IIR_h__
#define __IIR_H__

#include <stdint.h>
template <typename dataT, typename bufferT>
class IIR
{	// IIR
	public:
		IIR(uint8_t FilterBits);
		void set(dataT val=0);
		void add(dataT val);
		dataT get(void);

	private:
		bufferT Buffer;
		uint8_t Filter;
};	// IIR

template <typename dataT, typename bufferT>
IIR<dataT,bufferT>::IIR(uint8_t FilterBits)
{	// IIR
	Buffer=0;
	Filter=FilterBits;
}	// IIR

template <typename dataT, typename bufferT>
void IIR<dataT,bufferT>::set(dataT val)
{	// set
	bool bPos;
	bPos=val>=0;
	if (!bPos)
	{	// negative
		Buffer=-val;
	}	// negative
	else
	{	// postive
		Buffer=val;
	}	// postive
	Buffer<<=Filter;
	if (!bPos)
	{	// negative
		Buffer=-Buffer;
	}	// negative
}	// set

template <typename dataT, typename bufferT>
void IIR<dataT,bufferT>::add(dataT val)
{	// add
	bool bPos;
	bufferT temp;
	bPos=Buffer>=0;

	// make positive
	if (!bPos)
	{	// negative
		Buffer=-Buffer;
	}	// negative

	// round
	temp=Buffer;
	temp+=1<<(Filter-1);

	// modify buffer
	temp>>=Filter;
	Buffer-=temp;

	// restore sign
	if (!bPos)
	{	// negative
		Buffer=-Buffer;
	}	// negative

    Buffer+=val;
}	// add

template <typename dataT, typename bufferT>
dataT IIR<dataT,bufferT>::get(void)
{	// get
	bool bPos;
	bufferT temp=Buffer;
	bPos=Buffer>=0;

	// make positive
	if (!bPos)
	{	// negative
		temp=-temp;
	}	// negative

	// round
	temp+=1<<(Filter-1);

	// shift
	temp>>=Filter;
	
	// restore sign
	if (!bPos)
	{	// negative
		temp=-temp;
	}	// negative

	return (dataT)temp;
}	// get



#endif // __IIR_H__
