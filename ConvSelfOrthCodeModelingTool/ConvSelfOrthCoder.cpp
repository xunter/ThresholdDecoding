#include "StdAfx.h"
#include "ConvSelfOrthCoder.h"


ConvSelfOrthCoder::ConvSelfOrthCoder(BinaryMatrix *polynomFactors, int polynomPower, const int &k0, const int &n0, const int &dataBlockLen)
{
	_polynomFactors = polynomFactors;
	_polynomPower = polynomPower;
	_n0 = n0;
	_k0 = k0;
	_dataBlockLen = dataBlockLen;
}


ConvSelfOrthCoder::~ConvSelfOrthCoder(void)
{
}

byte *ConvSelfOrthCoder::Encode(byte* src)
{
	int entireLen = (_polynomPower + 1) * _n0;
	int dataLen = (_polynomPower + 1) * _k0;
	int checkingLen = entireLen - dataLen;

	byte *entireBlock = new byte[entireLen];
	memcpy(entireBlock, src, sizeof(src));

	for (int i = 0; i < checkingLen; i++)
	{

	}

	return entireBlock;
}

byte *ConvSelfOrthCoder::Decode(byte* src)
{
	return src;
};

void ConvSelfOrthCoder::Init()
{

}

int ConvSelfOrthCoder::GetDataLen()
{

}

int ConvSelfOrthCoder::GetEntireLen()
{

}