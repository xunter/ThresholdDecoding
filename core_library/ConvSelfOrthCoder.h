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
		
	void DisplayDebugInfo(const char *label);
protected:
	ConvSelfOrthCoder();
	
	ConvSelfOrthCoder *_coderForDecoder;
	BinaryMatrix **_arrCoderRegistries;
	std::vector<BinaryMatrix *> *_syndromeRegistries;
	CoderDefinition *_coderDefinition;

	int _dmin;
	int _polynomPower;
	int _n0;
	int _k0;
	int _countCheckBits;
	bool *_arrBoolSource;
	bool *_arrBoolEncoded;

	virtual void DisplayDebugInfoExternalCoder(const char *label);
	virtual void DisplayDebugInfoCoderDataRegistry();

	virtual void SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, const bool &nextVal);

	void ShiftCoderRegistryRight(BinaryMatrix *coderRegistry);
	void ShiftSyndromeRegistryRight(BinaryMatrix *syndromeRegistry);
	void SetFirstItem(BinaryMatrix *coderRegistry, const bool &val);
	virtual bool CheckThresholdCondition(int indexData, std::vector<CoderDefinitionItem *> *vecCheckBranchItems = NULL);
	virtual bool CheckThresholdConditionSyndrome(int indexOutput, std::vector<bool> &syndromes, std::vector<bool> &additionalParts);
	void DropFlagsSyndromeRegistry(BinaryMatrix *syndromeRegistry, int indexDataBranch, int indexCheckBranch);
	void FilterSyndromeRegistriesForCondition(int indexCondition, std::vector<BinaryMatrix *> &vec);

	int GetEncodedBitsCount();
	int GetSizeCoderRegistry();
	int GetCountCoderRegistries();
	int GetSizeSyndromeRegistry();
	int GetCountSyndromeRegistries();

	virtual float ComputeThresholdValue(int indexData);

	void InitThresholdCoderCore(CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder);
	virtual void EncodeCore(bool *inputBits, std::vector<bool> &encodedBits);
	virtual void DecodeCore(bool *receivedBits, std::vector<bool> &decodedBits);

	void ShiftRegistryRight(BinaryMatrix *registry);
	void InitCoderRegistries();
	void InitSyndromeRegistries();
private:
};

}