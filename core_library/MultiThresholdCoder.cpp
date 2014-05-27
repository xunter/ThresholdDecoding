#include "MultiThresholdCoder.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

MultiThresholdCoder::MultiThresholdCoder(void)
{
	_firstSection = true;
}

MultiThresholdCoder::MultiThresholdCoder(int countDecoderSections, CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder)
{	
	_firstSection = true;
	_countDecoderSections = countDecoderSections;
	InitThresholdCoderCore(coderDefinition, polynomPower, k0, n0, createCoderForDecoder);
};

void MultiThresholdCoder::SetFirstSection(bool val) {
	_firstSection = val;
};

void MultiThresholdCoder::Init()
{
	ConvSelfOrthCoder::Init();

	if (IsHubDecoder()) {
		InitDecoderSectionsCoders();
	} else {
		InitDiffRegistries();

		_arrSyndromeInitVals = new bool[_n0 - _k0];
		_arrThresholdConditions = new bool[_k0];
		_arrDiffInitVals = new bool[_k0];
		for (int i = 0; i < _k0; i++) {
			_arrDiffInitVals[i] = false;
		}
	}
};

bool MultiThresholdCoder::IsHubDecoder() {
	return _countDecoderSections > 1;
};

void MultiThresholdCoder::InitDiffRegistries()
{
	_diffRegistries = new std::vector<BinaryMatrix *>();
	int sizeDiffRegistry = GetSizeDiffRegistry();
	for (int i = 0; i < _k0; i++) {
		BinaryMatrix *diffRegistry = BinaryMatrix::CreateVector(sizeDiffRegistry);
		_diffRegistries->push_back(diffRegistry);
	}
}

bool *MultiThresholdCoder::DecodeCore(bool *encodedBits) {
	bool *arrDecodedBits = null;
	if (IsHubDecoder()) {
		MultiThresholdCoder *prevCoder = null;
		MultiThresholdCoder *nextCoder = null;

		int countSyndromeRegistries = _n0 - _k0;

		bool diffVal = false;
		bool *arrDataVals = new bool[_k0];
		bool *arrDiffVals = new bool[_k0];
		bool *arrCondVals = new bool[_k0];
		bool *arrSyndVals = new bool[countSyndromeRegistries];
		
		bool *arrEncodedBits = ByteUtil::CopyBoolArray(encodedBits, _n0);

		for (int i = 0; i < _countDecoderSections; i++) {
			MultiThresholdCoder *currCoder = _decoderSectionsCoders->at(i);
			if (i < _countDecoderSections - 1) {
				nextCoder = _decoderSectionsCoders->at(i + 1);
			}
						
			if (prevCoder != null) {				
				for (int j = 0; j < _k0; j++) {
					bool cond = arrCondVals[j];
					bool diff = arrDiffVals[j];
					bool data = arrDataVals[j];
					bool synd = arrSyndVals[j];
					if (cond) {
						diff = !diff;
					}

					currCoder->_arrDiffInitVals[j] = diff;
					currCoder->_arrSyndromeInitVals[j] = synd;

					arrEncodedBits[j] = data;
				}
			}
						
			bool *decodedBits = currCoder->DecodeCore(arrEncodedBits);

			for (int j = 0; j < _k0; j++) {
				arrDataVals[j] = decodedBits[j];
			}
								
			for (int j = 0; j < _k0; j++) {
				BinaryMatrix *diffMatrix = currCoder->_diffRegistries->at(j);
				arrDiffVals[j] = diffMatrix->GetLastZeroRowItem();
			}
				
			for (int j = 0; j < _k0; j++) {
				arrCondVals[j] = currCoder->_arrThresholdConditions[j];
			}
				
			for (int j = 0; j < countSyndromeRegistries; j++) {
				BinaryMatrix *synd = currCoder->_syndromeRegistries->at(j);
				arrSyndVals[j] = synd->GetItem(0, synd->GetColCount() - 1);
			}

			prevCoder = currCoder;

			if (nextCoder == null) {
				arrDecodedBits = decodedBits;
			}
		}

		delete [] arrSyndVals;
		delete [] arrCondVals;
		delete [] arrDiffVals;
		delete [] arrDataVals;
	} else {

		for (int i = 0; i < _k0; i++) {
			BinaryMatrix *diffReg = _diffRegistries->at(i);
			bool initVal = _arrDiffInitVals[i];
			ShiftDiffRegistryRight(diffReg);
			diffReg->SetItem(0, 0, initVal);
		}

		arrDecodedBits = ConvSelfOrthCoder::DecodeCore(encodedBits);
	}
	return arrDecodedBits;
};

int MultiThresholdCoder::GetSizeDiffRegistry()
{
	return GetSizeCoderRegistry();
}

void MultiThresholdCoder::ShiftDiffRegistryRight(BinaryMatrix *diffRegistry)
{
	ShiftRegistryRight(diffRegistry);
}

void MultiThresholdCoder::InitDecoderSectionsCoders() {	
	_decoderSectionsCoders = new std::vector<MultiThresholdCoder *>();
	for (int i = 0; i < _countDecoderSections; i++) {
		MultiThresholdCoder *coder = new MultiThresholdCoder(1, _coderDefinition, _polynomPower, _k0, _n0, true);
		coder->_firstSection = false;
		if (i == 0) {
			coder->_firstSection = true;
		}
		_decoderSectionsCoders->push_back(coder);
	}
};


void MultiThresholdCoder::SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, bool nextVal) {
	ShiftSyndromeRegistryRight(syndrome);
	if (_firstSection) {
		syndrome->SetItem(0, 0, nextVal);
	} else {
		bool initVal = _arrSyndromeInitVals[indexBit];
		syndrome->SetItem(0, 0, initVal);
	}
};

bool MultiThresholdCoder::CheckThresholdConditionSyndrome(int indexOutput, std::vector<bool> &syndromes, std::vector<bool> &additionalParts) {
	
	BinaryMatrix *diffRegistry = _diffRegistries->at(indexOutput);
	int sizeDiffRegistry = GetSizeDiffRegistry();
	bool diffVal = diffRegistry->GetItem(0, sizeDiffRegistry - 1);

	additionalParts.push_back(diffVal);
	bool condition = ConvSelfOrthCoder::CheckThresholdConditionSyndrome(indexOutput, syndromes, additionalParts);
	return condition;
};

bool MultiThresholdCoder::CheckThresholdCondition(int indexOutput) {
	bool baseCheckResult = ConvSelfOrthCoder::CheckThresholdCondition(indexOutput);
	bool resultVal = baseCheckResult;
	_arrThresholdConditions[indexOutput] = resultVal;
	return baseCheckResult;
};

MultiThresholdCoder::~MultiThresholdCoder(void)
{
	delete [] _arrThresholdConditions;
	if (_arrCoderRegistries != null) 
	{
		delete [] _arrCoderRegistries;
	}
	if (_arrDiffInitVals != null) {
		delete [] _arrDiffInitVals;
	}
	if (_arrSyndromeInitVals != null) {
		delete [] _arrSyndromeInitVals;
	}
}

}