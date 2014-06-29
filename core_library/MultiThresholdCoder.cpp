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

		int sizeSyndromeInitVals = _n0 - _k0;
		_arrSyndromeInitVals = new bool[sizeSyndromeInitVals];
		_arrThresholdConditions = new bool[_k0];
		_arrDiffInitVals = new bool[_k0];
		for (int i = 0; i < _k0; i++) {
			_arrDiffInitVals[i] = false;
			_arrThresholdConditions[i] = false;
		}
		for (int i = 0; i < sizeSyndromeInitVals; i++) {
			_arrSyndromeInitVals[i] = false;
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

void MultiThresholdCoder::DecodeCore(bool *encodedBits, std::vector<bool> &vecDecoded) {
	if (IsHubDecoder()) {
		MultiThresholdCoder *prevCoder = null;
		MultiThresholdCoder *nextCoder = null;

		int countSyndromeRegistries = _n0 - _k0;

		bool diffVal = false;

		std::vector<bool> vecDataVals(_k0, false);
		std::vector<bool> vecDiffVals(_k0, false);
		std::vector<bool> vecCondVals(_k0, false);
		std::vector<bool> vecSyndVals(countSyndromeRegistries, false);
		
		bool *arrEncodedBits = ByteUtil::CopyBoolArray(encodedBits, _n0);

		for (int i = 0; i < _countDecoderSections; i++) {
			MultiThresholdCoder *currCoder = _decoderSectionsCoders->at(i);
			if (i < _countDecoderSections - 1) nextCoder = _decoderSectionsCoders->at(i + 1);
			else nextCoder = null;
						
			if (prevCoder != null) {				
				for (int j = 0; j < _k0; j++) {
					const bool &cond = vecCondVals[j];
					bool diff = ByteUtil::Xor(vecDiffVals[j], cond);
					const bool &data = vecDataVals[j];

					currCoder->_arrDiffInitVals[j] = diff;

					arrEncodedBits[j] = data;
				}
				for (int j = 0; j < countSyndromeRegistries; j++) {
					bool synd = vecSyndVals[j];
					currCoder->_arrSyndromeInitVals[j] = synd;
				}
			}
					
			std::vector<bool> vecDecodedBitsCurrCoder(_k0, false);
			currCoder->DecodeCore(arrEncodedBits, vecDecodedBitsCurrCoder);

			for (int j = 0; j < _k0; j++) {
				vecDataVals[j] = vecDecodedBitsCurrCoder[j];
			}
								
			for (int j = 0; j < _k0; j++) {
				BinaryMatrix *diffMatrix = currCoder->_diffRegistries->at(j);
				vecDiffVals[j] = diffMatrix->GetLastZeroRowItem();
			}
				
			for (int j = 0; j < _k0; j++) {
				vecCondVals[j] = currCoder->_arrThresholdConditions[j];
			}
				
			for (int j = 0; j < countSyndromeRegistries; j++) {
				BinaryMatrix *synd = currCoder->_syndromeRegistries->at(j);
				vecSyndVals[j] = synd->GetItem(0, synd->GetColCount() - 1);
			}

			prevCoder = currCoder;

			if (nextCoder == null) {
				for (int j = 0; j < _k0; j++) vecDecoded[j] = vecDecodedBitsCurrCoder[j];
			}
		}
		delete [] arrEncodedBits;
	} else {

		for (int i = 0; i < _k0; i++) {
			BinaryMatrix *diffReg = _diffRegistries->at(i);
			bool initVal = _arrDiffInitVals[i];
			ShiftDiffRegistryRight(diffReg);
			diffReg->SetItem(0, 0, initVal);
		}

		ConvSelfOrthCoder::DecodeCore(encodedBits, vecDecoded);
	}
};

void MultiThresholdCoder::DisplayDebugInfo(const char *label) {
	
	if (IsHubDecoder()) {
		DisplayDebugInfoCoderDataRegistry();

		for (int i = 0; i < _decoderSectionsCoders->size(); i++) {
			char nbuf[256];
			itoa(i, nbuf, 10);
			std::string labelReg = "Section coder ";
			labelReg = labelReg + nbuf;
			_decoderSectionsCoders->at(i)->DisplayDebugInfo(labelReg.c_str());
		}	
	} else {
		ConvSelfOrthCoder::DisplayDebugInfo(label);
	}
};

void MultiThresholdCoder::DisplayDebugInfoExternalCoder(const char *label) {	
	ConvSelfOrthCoder::DisplayDebugInfoExternalCoder(label);
	int countDiffRegs = GetCountCoderRegistries();
	
	char nbuf[256];
	for (int i = 0; i < countDiffRegs; i++) {
		itoa(i, nbuf, 10);
		std::string labelReg = "Decoder diff registry ";
		labelReg = labelReg + nbuf;
		_diffRegistries->at(i)->DisplayConsole(labelReg.c_str());
	}
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
		coder->Init();
		_decoderSectionsCoders->push_back(coder);
	}
};


void MultiThresholdCoder::SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, const bool &nextVal) {
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

bool MultiThresholdCoder::CheckThresholdCondition(int indexData, std::vector<CoderDefinitionItem *> *vecCheckBranchItems) {
	bool baseCheckResult = ConvSelfOrthCoder::CheckThresholdCondition(indexData);
	bool resultVal = baseCheckResult;
	_arrThresholdConditions[indexData] = resultVal;
	return baseCheckResult;
};

MultiThresholdCoder::~MultiThresholdCoder(void)
{
	if (IsHubDecoder()) {
		for (int i = 0; i < _countDecoderSections; i++) {
			delete _decoderSectionsCoders->at(i);
		}
		delete _decoderSectionsCoders;
	} else {
		delete [] _arrThresholdConditions;
		delete [] _arrDiffInitVals;
		delete [] _arrSyndromeInitVals;

		for (int i = 0; i < _diffRegistries->size(); i++) delete _diffRegistries->at(i);
		delete _diffRegistries;
	}

}

}