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
	
	virtual void Init();
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
protected:
	ConvSelfOrthCoder();
	
	ConvSelfOrthCoder *_coderForDecoder;
	BinaryMatrix **_arrCoderRegistries;
	std::vector<BinaryMatrix *> *_syndromeRegistries;
	CoderDefinition *_coderDefinition;

	int _polynomPower;
	int _n0;
	int _k0;
	bool *_arrBoolSource;
	bool *_arrBoolEncoded;

	virtual void SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, bool nextVal);

	void ShiftCoderRegistryRight(BinaryMatrix *coderRegistry);
	void ShiftSyndromeRegistryRight(BinaryMatrix *syndromeRegistry);
	void SetFirstItem(BinaryMatrix *coderRegistry, bool &val);
	virtual bool CheckThresholdCondition(int indexOutput);
	bool CheckThresholdConditionSyndrome(int indexOutput, std::vector<bool> &syndromes, std::vector<bool> &additionalParts);
	void DropFlagsSyndromeRegistry(BinaryMatrix *syndromeRegistry, int indexBranch);
	std::vector<BinaryMatrix *> *FilterSyndromeRegistriesForCondition(int indexCondition);

	int GetEncodedBitsCount();
	int GetSizeCoderRegistry();
	int GetCountCoderRegistries();
	int GetSizeSyndromeRegistry();

	void InitThresholdCoderCore(CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder);
	bool *EncodeCore(bool *inputBits);

	virtual bool *DecodeCore(bool *encodedBits);

	void ShiftRegistryRight(BinaryMatrix *registry);
	void InitCoderRegistries();
	void InitSyndromeRegistries();
private:
};

}