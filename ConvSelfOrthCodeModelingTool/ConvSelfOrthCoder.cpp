#include "StdAfx.h"
#include "ConvSelfOrthCoder.h"


ConvSelfOrthCoder::ConvSelfOrthCoder(BinaryMatrix *polynomFactors, int polynomPower)
{
	_polynomFactors = polynomFactors;
	_polynomPower = polynomPower;
}


ConvSelfOrthCoder::~ConvSelfOrthCoder(void)
{
}

byte *ConvSelfOrthCoder::Encode(byte* src)
{
	return src;
}

byte *ConvSelfOrthCoder::Decode(byte* src)
{
	return src;
};
