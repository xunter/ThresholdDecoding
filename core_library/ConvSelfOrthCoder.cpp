#pragma once

#include "ConvSelfOrthCoder.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	
ConvSelfOrthCoder::ConvSelfOrthCoder(CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder)
{
	InitThresholdCoderCore(coderDefinition, polynomPower, k0, n0, createCoderForDecoder);
}

void ConvSelfOrthCoder::InitThresholdCoderCore(CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder)
{
	_coderDefinition = coderDefinition;
	_polynomPower = polynomPower;
	_n0 = n0;
	_k0 = k0;

	_arrBoolEncoded = new bool[_n0];
	_arrBoolSource = new bool[_k0];

	_coderForDecoder = null;
	if (createCoderForDecoder)
	{
		_coderForDecoder = new ConvSelfOrthCoder(coderDefinition, polynomPower, k0, n0, false);
		_coderForDecoder->Init();
	}
}


ConvSelfOrthCoder::ConvSelfOrthCoder()
{
}


ConvSelfOrthCoder::~ConvSelfOrthCoder(void)
{
	int countCoderRegistries = GetCountCoderRegistries();
	for (int i = 0; i < countCoderRegistries; i++) {
		BinaryMatrix *coderRegistry = _arrCoderRegistries[i];
		BaseClass::Clean(coderRegistry);	
	}
	delete [] _arrCoderRegistries;
	int countSyndromeRegistries = _k0;
	for (int i = 0; i < countSyndromeRegistries; i++) {
		BinaryMatrix *syndromeRegistry = _syndromeRegistries->at(i);
		BaseClass::Clean(syndromeRegistry);
	}
	delete _syndromeRegistries;
		
	delete [] _arrBoolEncoded;
	delete [] _arrBoolSource;

	if (_coderForDecoder != null)
	{
		BaseClass::Clean(_coderForDecoder);
	}

}

byte *ConvSelfOrthCoder::Encode(byte* src)
{
	int countBits = _k0;
	
	ByteUtil::ConvertBitsToBoolArray(src, countBits, _arrBoolSource);
	
	bool *arrBitsEncodedOutput = EncodeCore(_arrBoolSource);

	int lengthOfEncodedData;
	byte *encodedData = ByteUtil::StoreBoolArrayAsBytes(arrBitsEncodedOutput, _n0, lengthOfEncodedData);
	
	delete [] arrBitsEncodedOutput;

	return encodedData;
}

bool *ConvSelfOrthCoder::EncodeCore(bool *inputBits)
{
	bool *arrBitsEncodedOutput = new bool[_n0];
	int countBits = _k0;
	bool *boolBits = inputBits;
	for (int i = 0; i < countBits; i++) {
		bool val = boolBits[i];
		arrBitsEncodedOutput[i] = val;
		BinaryMatrix *registry = _arrCoderRegistries[i];
		ShiftCoderRegistryRight(registry);
		SetFirstItem(registry, val);
		//registry->DisplayConsole("coder registry");
	}

	int countCheckBits = _coderDefinition->GetCountCheckOutputs();

	for (int i = 0; i < countCheckBits; i++) {
		bool checkBit = false;
		std::vector<CoderDefinitionItem> *checkOnlyItems = _coderDefinition->FilterItemsByOutputBranchIndex(i);
		for (int j = 0; j < checkOnlyItems->size(); j++) {
			CoderDefinitionItem &checkItem = checkOnlyItems->at(j);
			int indexInput = checkItem.i - 1;
			BinaryMatrix *coderRegistry = _arrCoderRegistries[indexInput];
			for (int k = 0; k < checkItem.powersPolynom.size(); k++) {
				int &power = checkItem.powersPolynom.at(k);
				bool registryCellVal = coderRegistry->GetItem(0, power);
				checkBit = ByteUtil::Xor(checkBit, registryCellVal);
			}
		}
		delete checkOnlyItems;
		int indexCheckBit = _k0 + i;
		arrBitsEncodedOutput[indexCheckBit] = checkBit;
	}
	return arrBitsEncodedOutput;
}

byte *ConvSelfOrthCoder::Decode(byte* src)
{	
	int countEncodedBits = _n0;
	ByteUtil::ConvertBitsToBoolArray(src, countEncodedBits, _arrBoolEncoded);
	
	bool *arrDecodedBits = DecodeCore(_arrBoolEncoded);

	int lengthOfDecodedData;
	byte *decodedData = ByteUtil::StoreBoolArrayAsBytes(arrDecodedBits, _k0, lengthOfDecodedData);
	
	delete [] arrDecodedBits;

	return decodedData;
};

void ConvSelfOrthCoder::SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, bool nextVal) {
	ShiftSyndromeRegistryRight(syndrome);
	syndrome->SetItem(0, 0, nextVal);
};

bool *ConvSelfOrthCoder::DecodeCore(bool *encodedBits) {
	
	bool *arrDecodedBits = new bool[_k0];
	
	bool checkingBitFromCoderForDecoder;
	bool encodedBitFromCoderForDecoder;
	bool *encodedBitsDecoderCoder = _coderForDecoder->EncodeCore(encodedBits);

	int sizeSyndromeRegistry = GetSizeSyndromeRegistry();
	int countSyndromeRegistries = _n0 - _k0;
	for (int i = 0; i < countSyndromeRegistries; i++) {
		int indexCheckBit = _k0 + i;
		bool checkBitDecoderCoder = encodedBitsDecoderCoder[indexCheckBit];
		bool incomingCheckBit = encodedBits[indexCheckBit];
		bool currentSyndrome = ByteUtil::Xor(checkBitDecoderCoder, incomingCheckBit);
		BinaryMatrix *currSyndromeRegistry = _syndromeRegistries->at(i);

		ShiftSyndromeRegistryRight(currSyndromeRegistry);
		currSyndromeRegistry->SetItem(0, 0, currentSyndrome);
		
		//currSyndromeRegistry->DisplayConsole("syndrome registry");
	}

	for (int i = 0; i < _k0; i++) {
		bool thresholdConditionResult = CheckThresholdCondition(i);
		BinaryMatrix *targetDecoderCoderRegistry = _coderForDecoder->_arrCoderRegistries[i];
		bool lastBitInRegistry = targetDecoderCoderRegistry->GetItem(0, targetDecoderCoderRegistry->GetColCount() - 1);
		bool decodedBit = ByteUtil::Xor(thresholdConditionResult, lastBitInRegistry);

		if (thresholdConditionResult) {
			std::vector<BinaryMatrix *> *syndromes = FilterSyndromeRegistriesForCondition(i);
			for (int j = 0; j < syndromes->size(); j++) {
				BinaryMatrix *syndrome = syndromes->at(j);
				DropFlagsSyndromeRegistry(syndrome, i);
			}
			delete syndromes;
		}

		arrDecodedBits[i] = decodedBit;
	}

	return arrDecodedBits;
};

std::vector<BinaryMatrix *> *ConvSelfOrthCoder::FilterSyndromeRegistriesForCondition(int indexCondition) {
	std::vector<BinaryMatrix *> *vec = new std::vector<BinaryMatrix *>();
	BinaryMatrix *syndrome = _syndromeRegistries->at(indexCondition);
	vec->push_back(syndrome);
	return vec;
};

void ConvSelfOrthCoder::DropFlagsSyndromeRegistry(BinaryMatrix *syndromeRegistry, int indexBranch) {
	//fill syndromes
	int countCheckBits = _coderDefinition->GetCountCheckOutputs();
	for (int i = 0; i < countCheckBits; i++) {
		std::vector<CoderDefinitionItem> *checkOnlyItems = _coderDefinition->FilterItemsByOutputBranchIndex(i);
		for (int j = 0; j < checkOnlyItems->size(); j++) {
			CoderDefinitionItem &checkItem = checkOnlyItems->at(j);
			int indexInput = checkItem.i - 1;
			BinaryMatrix *syndromeRegistry = _syndromeRegistries->at(indexInput);
			for (int k = 0; k < checkItem.powersPolynom.size(); k++) {
				int &power = checkItem.powersPolynom.at(k);
				int indexCell = syndromeRegistry->GetColCount() - (power + 1);
				syndromeRegistry->SetItem(0, indexCell, false);
			}
		}
		delete checkOnlyItems;
	}
};

bool ConvSelfOrthCoder::CheckThresholdCondition(int indexOutput)
{
	std::vector<bool> syndromes;

	//fill syndromes
	int countCheckBits = _coderDefinition->GetCountCheckOutputs();
	for (int i = 0; i < countCheckBits; i++) {
		std::vector<CoderDefinitionItem> *checkOnlyItems = _coderDefinition->FilterItemsByOutputBranchIndex(i);
		for (int j = 0; j < checkOnlyItems->size(); j++) {
			CoderDefinitionItem &checkItem = checkOnlyItems->at(j);
			int indexInput = checkItem.i - 1;
			BinaryMatrix *syndromeRegistry = _syndromeRegistries->at(indexInput);
			for (int k = 0; k < checkItem.powersPolynom.size(); k++) {
				int &power = checkItem.powersPolynom.at(k);
				int indexCell = syndromeRegistry->GetColCount() - (power + 1);
				bool syndromeVal = syndromeRegistry->GetItem(0, indexCell);
				syndromes.push_back(syndromeVal);
			}
		}
		delete checkOnlyItems;
	}

	std::vector<bool> additionalParts;
	bool condition = CheckThresholdConditionSyndrome(indexOutput, syndromes, additionalParts);

	return condition;
}

bool ConvSelfOrthCoder::CheckThresholdConditionSyndrome(int indexOutput, std::vector<bool> &syndromes, std::vector<bool> &additionalParts) {
	
	std::vector<bool> all;
	for (int i = 0; i < syndromes.size(); i++) {
		all.push_back(syndromes.at(i));
	}
	for (int i = 0; i < additionalParts.size(); i++) {
		all.push_back(additionalParts.at(i));
	}
	float t = (float)all.size() / 2;
	int countOfIdentities = 0;
	for (int i = 0; i < all.size(); i++) {
		if (all.at(i) == true) {
			countOfIdentities++;
		}
	}
	return (float)countOfIdentities > t;
};

void ConvSelfOrthCoder::SetFirstItem(BinaryMatrix *coderRegistry, bool &val) {
	coderRegistry->SetItem(0, 0, val);
}

void ConvSelfOrthCoder::ShiftCoderRegistryRight(BinaryMatrix *coderRegistry)
{
	ShiftRegistryRight(coderRegistry);
}

void ConvSelfOrthCoder::ShiftSyndromeRegistryRight(BinaryMatrix *syndromeRegistry)
{
	ShiftRegistryRight(syndromeRegistry);
}

void ConvSelfOrthCoder::ShiftRegistryRight(BinaryMatrix *registry)
{
	registry->ShiftRightOnce();
}

void ConvSelfOrthCoder::Init()
{
	InitCoderRegistries();
	InitSyndromeRegistries();
}

void ConvSelfOrthCoder::InitSyndromeRegistries() {
	int sizeSyndromeRegistry = GetSizeSyndromeRegistry();
	_syndromeRegistries = new std::vector<BinaryMatrix*>();
	int countSyndromeRegistries = _n0;
	for (int i = 0; i < countSyndromeRegistries; i++) {
		BinaryMatrix *syndromeRegistry = BinaryMatrix::CreateVector(sizeSyndromeRegistry);
		_syndromeRegistries->push_back(syndromeRegistry);
	}
};

void ConvSelfOrthCoder::InitCoderRegistries()
{
	int countRegistries = GetCountCoderRegistries();
	int lenCoderRegistry = GetSizeCoderRegistry();
	_arrCoderRegistries = new BinaryMatrix*[countRegistries];
	for (int i = 0; i < countRegistries; i++) {
		BinaryMatrix *coderRegistry = BinaryMatrix::CreateVector(lenCoderRegistry);
		_arrCoderRegistries[i] = coderRegistry;
	}
}

int ConvSelfOrthCoder::GetSizeSyndromeRegistry() {
	return _polynomPower + 1;
};

int ConvSelfOrthCoder::GetSizeCoderRegistry() {
	return _polynomPower + 1;
};

int ConvSelfOrthCoder::GetCountCoderRegistries() {
	return _k0;
};

int ConvSelfOrthCoder::GetEncodedBitsCount() {
	return _n0;
}
}