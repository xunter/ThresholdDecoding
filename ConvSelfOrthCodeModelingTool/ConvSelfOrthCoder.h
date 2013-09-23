#pragma once
#include "coder.h"
#include "BinaryMatrix.h"

class ConvSelfOrthCoder :
	public Coder
{
public:
	ConvSelfOrthCoder(BinaryMatrix *polynomFactors, int polynomPower, const int &k0, const int &n0, const int &dataBlockLen);
	~ConvSelfOrthCoder(void);
	
	void Init();
	int GetDataLen();
	int GetEntireLen();
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
private:
	BinaryMatrix *_polynomFactors;
	int _polynomPower;
	int _n0;
	int _k0;
	int _dataBlockLen;
};

