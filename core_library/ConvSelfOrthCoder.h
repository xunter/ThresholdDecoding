#pragma once
#include "core_library.h"
#include "coder.h"

#include "BinaryMatrix.h"
#include "CoderDefinitions.h"


namespace ThresholdDecoding {

class ConvSelfOrthCoder :
	public Coder
{
public:
	ConvSelfOrthCoder(CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder);
	~ConvSelfOrthCoder(void);
	
	void Init();
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
protected:
	bool *EncodeCore(bool *inputBits);
	void ShiftRegistryRight(BinaryMatrix *registry);
	void InitCoderRegistries();
	void InitSyndromeRegistries();
private:
	ConvSelfOrthCoder *_coderForDecoder;
	BinaryMatrix **_arrCoderRegistries;
	std::vector<BinaryMatrix *> *_syndromeRegistries;
	CoderDefinition *_coderDefinition;

	int _polynomPower;
	int _n0;
	int _k0;

	void ShiftCoderRegistryRight(BinaryMatrix *coderRegistry);
	void ShiftSyndromeRegistryRight(BinaryMatrix *syndromeRegistry);
	void SetFirstItem(BinaryMatrix *coderRegistry, bool &val);
	bool CheckThresholdCondition(int indexOutput);

	int GetEncodedBitsCount();
	int GetSizeCoderRegistry();
	int GetCountCoderRegistries();
	int GetSizeSyndromeRegistry();
};

}