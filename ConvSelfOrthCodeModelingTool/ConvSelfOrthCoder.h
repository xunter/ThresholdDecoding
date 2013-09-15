#pragma once
#include "coder.h"
#include "BinaryMatrix.h"

class ConvSelfOrthCoder :
	public Coder
{
public:
	ConvSelfOrthCoder(BinaryMatrix *polynomFactors, int polynomPower);
	~ConvSelfOrthCoder(void);
	
	void Init();
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
private:
	BinaryMatrix *_polynomFactors;
	int _polynomPower;
};

