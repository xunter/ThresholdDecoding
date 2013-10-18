#pragma once
#include "coder.h"

#include "BinaryMatrix.h"


namespace ThresholdDecoding {

class ConvSelfOrthCoder :
	public Coder
{
public:
	ConvSelfOrthCoder(BinaryMatrix *polynomFactors, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder);
	~ConvSelfOrthCoder(void);
	
	void Init();
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
protected:
	void EncodeCore(bool bit, bool &encodedBit, bool &checkingBit);
	void ShiftRegistryRight(BinaryMatrix *registry);
	void InitHIdentity();
	void InitHdelta();
	void InitH();
private:
	ConvSelfOrthCoder *_coderForDecoder;
	BinaryMatrix *_coderRegistry;
	BinaryMatrix *_syndromeRegistry;
	BinaryMatrix *_polynomFactors;
	BinaryMatrix *_Hdelta;
	BinaryMatrix *_Hidentity;
	BinaryMatrix *_H;

	int _polynomPower;
	int _n0;
	int _k0;

	void ShiftCoderRegistryRight();
	void ShiftSyndromeRegistryRight();
	void SetFirstItem(bool &val);
	bool ComputeCurrentCheckingBit();
	bool CheckThresholdCondition(bool sumOfSyndromes);

	int GetEncodedBitsCount();
};

}