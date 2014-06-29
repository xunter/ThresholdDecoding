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
	_dmin = 0;

	_arrBoolEncoded = new bool[_n0];
	_arrBoolSource = new bool[_k0];
		
	_countCheckBits = _coderDefinition->GetCountCheckOutputs();

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
	
	std::vector<bool> vecEncodedBits(_n0, false);
	EncodeCore(_arrBoolSource, vecEncodedBits);

	int lengthOfEncodedData;
	byte *encodedData = ByteUtil::StoreBoolVectorAsBytes(vecEncodedBits, lengthOfEncodedData);
	
	return encodedData;
}

void ConvSelfOrthCoder::EncodeCore(bool *inputBits, std::vector<bool> &encodedBits)
{
	int countBits = _k0;
	for (int i = 0; i < countBits; i++) {
		const bool &val = inputBits[i];
		encodedBits[i] = val;
		BinaryMatrix *registry = _arrCoderRegistries[i];
		ShiftCoderRegistryRight(registry);
		SetFirstItem(registry, val);
	}
	
	for (int i = 0; i < _countCheckBits; i++) {
		bool checkBit = false;
		std::vector<CoderDefinitionItem> checkOnlyItems;
		_coderDefinition->FilterItemsByOutputBranchIndex(i, checkOnlyItems);
		for (int j = 0; j < checkOnlyItems.size(); j++) {
			CoderDefinitionItem &checkItem = checkOnlyItems.at(j);
			int indexInput = checkItem.i - 1;
			BinaryMatrix *coderRegistry = _arrCoderRegistries[indexInput];
			for (int k = 0; k < checkItem.powersPolynom.size(); k++) {
				int &power = checkItem.powersPolynom.at(k);
				const bool &registryCellVal = coderRegistry->GetItem(0, power);
				checkBit = ByteUtil::Xor(checkBit, registryCellVal);
			}
		}
		int indexCheckBit = _k0 + i;
		encodedBits[indexCheckBit] = checkBit;
	}
}

byte *ConvSelfOrthCoder::Decode(byte* src)
{	
	int countEncodedBits = _n0;
	ByteUtil::ConvertBitsToBoolArray(src, countEncodedBits, _arrBoolEncoded);
	
	std::vector<bool> vecDecodedBits(_k0, false);
	DecodeCore(_arrBoolEncoded, vecDecodedBits);

	int lengthOfDecodedData;
	byte *decodedData = ByteUtil::StoreBoolVectorAsBytes(vecDecodedBits, lengthOfDecodedData);
	
	return decodedData;
};

void ConvSelfOrthCoder::SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, const bool &nextVal) {
	ShiftSyndromeRegistryRight(syndrome);
	syndrome->SetItem(0, 0, nextVal);
};

void ConvSelfOrthCoder::DecodeCore(bool *encodedBits, std::vector<bool> &decodedBits) {
		
	bool checkingBitFromCoderForDecoder;
	bool encodedBitFromCoderForDecoder;
	std::vector<bool> vecEncodedBitsInternalCoder(_n0, false);
	_coderForDecoder->EncodeCore(encodedBits, vecEncodedBitsInternalCoder);

	int sizeSyndromeRegistry = GetSizeSyndromeRegistry();
	int countSyndromeRegistries = GetCountSyndromeRegistries();
	for (int i = 0; i < countSyndromeRegistries; i++) {
		int indexCheckBit = _k0 + i;
		const bool &checkBitDecoderCoder = vecEncodedBitsInternalCoder[indexCheckBit];
		const bool &incomingCheckBit = encodedBits[indexCheckBit];
		bool currentSyndromeVal = ByteUtil::Xor(checkBitDecoderCoder, incomingCheckBit);
		BinaryMatrix *currSyndromeRegistry = _syndromeRegistries->at(i);

		SetNextSyndromeVal(i, currSyndromeRegistry, currentSyndromeVal);
	}

	for (int i = 0; i < _k0; i++) {
		std::vector<CoderDefinitionItem *> vecTargetCheckIndexes;
		_coderDefinition->GetItemsForDataBranchIndex(i, vecTargetCheckIndexes);
		bool thresholdConditionResult = CheckThresholdCondition(i, &vecTargetCheckIndexes);
		BinaryMatrix *targetDecoderCoderRegistry = _coderForDecoder->_arrCoderRegistries[i];
		const bool &lastBitInRegistry = targetDecoderCoderRegistry->GetItem(0, targetDecoderCoderRegistry->GetColCount() - 1);
		const bool &decodedBit = ByteUtil::Xor(thresholdConditionResult, lastBitInRegistry);

		if (thresholdConditionResult) {
			for (int j = 0; j < vecTargetCheckIndexes.size(); j++) {
				CoderDefinitionItem *eachItem = vecTargetCheckIndexes.at(j);
				int indexCheckBranch = eachItem->j - 1;
				BinaryMatrix *syndrome = _syndromeRegistries->at(indexCheckBranch);
				DropFlagsSyndromeRegistry(syndrome, i, indexCheckBranch);
			}
		}

		decodedBits[i] = decodedBit;
	}
};

void ConvSelfOrthCoder::FilterSyndromeRegistriesForCondition(int indexCondition, std::vector<BinaryMatrix *> &vec) {
	std::vector<CoderDefinitionItem *> vecItems;
	_coderDefinition->GetItemsForDataBranchIndex(indexCondition, vecItems);
	for (int i = 0; i < vecItems.size(); i++) {
		CoderDefinitionItem *item = vecItems[i];
		int indexCheckBranch = item->j - 1;
		BinaryMatrix *eachSyndrome = _syndromeRegistries->at(indexCheckBranch);
		vec.push_back(eachSyndrome);
	}
};

void ConvSelfOrthCoder::DropFlagsSyndromeRegistry(BinaryMatrix *syndromeRegistry, int indexDataBranch, int indexCheckBranch) {
	CoderDefinitionItem *checkItem = _coderDefinition->FindItemByDataCheckIndexes(indexDataBranch, indexCheckBranch);
	for (int k = 0; k < checkItem->powersPolynom.size(); k++) {
		int &power = checkItem->powersPolynom.at(k);
		int indexCell = syndromeRegistry->GetColCount() - (power + 1);
		syndromeRegistry->SetItem(0, indexCell, false);
	}
};

bool ConvSelfOrthCoder::CheckThresholdCondition(int indexOutput, std::vector<CoderDefinitionItem *> *vecCheckBranchItems)
{
	int indexTargetDataBranch = indexOutput;
	std::vector<bool> syndromes;
	
	std::vector<CoderDefinitionItem *> vecTargetCheckIndexes;
	if (vecCheckBranchItems != NULL) vecTargetCheckIndexes = *vecCheckBranchItems;
	else _coderDefinition->GetItemsForDataBranchIndex(indexTargetDataBranch, vecTargetCheckIndexes);
	
	//fill syndromes
	int countCheckBits = vecTargetCheckIndexes.size();
	for (int i = 0; i < countCheckBits; i++) {
		CoderDefinitionItem *checkItem = vecTargetCheckIndexes.at(i);
		int indexTargetCheckBranch = checkItem->j - 1;
		BinaryMatrix *syndromeRegistry = _syndromeRegistries->at(indexTargetCheckBranch);
		for (int k = 0; k < checkItem->powersPolynom.size(); k++) {
			int &power = checkItem->powersPolynom.at(k);
			int indexCell = syndromeRegistry->GetColCount() - (power + 1);
			bool syndromeVal = syndromeRegistry->GetItem(0, indexCell);
			syndromes.push_back(syndromeVal);
		}
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
	float t = ComputeThresholdValue(indexOutput);
	int countOfIdentities = 0;
	for (int i = 0; i < all.size(); i++) {
		if (all.at(i) == true) {
			countOfIdentities++;
		}
	}
	return (float)countOfIdentities > t;
};

void ConvSelfOrthCoder::SetFirstItem(BinaryMatrix *coderRegistry, const bool &val) {
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
	_dmin = _coderDefinition->GetDmin();
}

void ConvSelfOrthCoder::InitSyndromeRegistries() {
	int sizeSyndromeRegistry = GetSizeSyndromeRegistry();
	int countSyndromeRegistries = GetCountSyndromeRegistries();
	_syndromeRegistries = new std::vector<BinaryMatrix*>();
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

int ConvSelfOrthCoder::GetCountSyndromeRegistries() {
	return _n0 - _k0;
};

int ConvSelfOrthCoder::GetEncodedBitsCount() {
	return _n0;
};

float ConvSelfOrthCoder::ComputeThresholdValue(int indexData) {
	float t = (float)(_dmin - 1) / 2;
	return t;
};


void ConvSelfOrthCoder::DisplayDebugInfoCoderDataRegistry() {
	char nbuf[256];
	int countCoderRegs = GetCountCoderRegistries();
	for (int i = 0; i < countCoderRegs; i++) { 
		itoa(i, nbuf, 10);
		std::string labelReg = "Coder data registry ";
		labelReg = labelReg + nbuf;
		_arrCoderRegistries[i]->DisplayConsole(labelReg.c_str());
	}
};

void ConvSelfOrthCoder::DisplayDebugInfoExternalCoder(const char *label) {	
	
	std::cout << "Coder " << label << std::endl;

	char nbuf[256];
	int countCoderRegs = GetCountCoderRegistries();
	DisplayDebugInfoCoderDataRegistry();
	
	for (int i = 0; i < countCoderRegs; i++) {
		itoa(i, nbuf, 10);
		std::string labelReg = "Decoder data registry ";
		labelReg = labelReg + nbuf;
		_coderForDecoder->_arrCoderRegistries[i]->DisplayConsole(labelReg.c_str());
	}

	int countSyndromeRegistries = GetCountSyndromeRegistries();
	for (int i = 0; i < countSyndromeRegistries; i++) {
		itoa(i, nbuf, 10);
		std::string labelReg = "Decoder syndrome registry ";
		labelReg = labelReg + nbuf;
		_syndromeRegistries->at(i)->DisplayConsole(labelReg.c_str());
	}
};

void ConvSelfOrthCoder::DisplayDebugInfo(const char *label) {
	if (_coderForDecoder != null) {	
		DisplayDebugInfoExternalCoder(label);
	}
};
}