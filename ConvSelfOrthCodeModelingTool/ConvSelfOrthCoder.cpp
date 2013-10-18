#pragma once
#include "StdAfx.h"
#include "ConvSelfOrthCoder.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	
ConvSelfOrthCoder::ConvSelfOrthCoder(BinaryMatrix *polynomFactors, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder)
{
	_polynomFactors = polynomFactors;
	_polynomPower = polynomPower;
	_n0 = n0;
	_k0 = k0;
	//_syndromeRegistry = new BinaryMatrix(1, polynomPower + 1);

	_syndromeRegistry = null;
	_coderForDecoder = null;
	if (createCoderForDecoder)
	{
		_coderForDecoder = new ConvSelfOrthCoder(polynomFactors, polynomPower, k0, n0, false);
		_coderForDecoder->Init();
	}
}

ConvSelfOrthCoder::~ConvSelfOrthCoder(void)
{
	BaseClass::Clean(_coderRegistry);
	BaseClass::Clean(_syndromeRegistry);

	/*
	BaseClass::Clean(_H);
	BaseClass::Clean(_Hdelta);
	BaseClass::Clean(_Hidentity);
	*/

	if (_coderForDecoder != null)
	{
		BaseClass::Clean(_coderForDecoder);
	}
}

byte *ConvSelfOrthCoder::Encode(byte* src)
{
	bool bit = ByteUtil::IsBitSettedInByte(src[0], 0);

	bool encodedBit;
	bool checkingBit;
	EncodeCore(bit, encodedBit, checkingBit);

	bool *encodedDataAsBoolArray = new bool[2];
	encodedDataAsBoolArray[0] = encodedBit;
	encodedDataAsBoolArray[1] = checkingBit;

	int lengthOfEncodedData;
	byte *encodedData = ByteUtil::StoreBoolArrayAsBytes(encodedDataAsBoolArray, 2, lengthOfEncodedData);

	delete [] encodedDataAsBoolArray;

	return encodedData;
}

void ConvSelfOrthCoder::EncodeCore(bool bit, bool &encodedBit, bool &checkingBit)
{
	ShiftRegistryRight(_coderRegistry);
	SetFirstItem(bit);
	checkingBit = ComputeCurrentCheckingBit();
	encodedBit = bit;
}

bool ConvSelfOrthCoder::ComputeCurrentCheckingBit()
{
	bool checkingBit = false;
	for (int i = 0; i < _polynomPower; i++)
	{
		bool eachPolynomItem = _polynomFactors->GetItem(0, i);
		if (eachPolynomItem)
		{
			checkingBit = checkingBit ^ _coderRegistry->GetItem(0, i);
		}
	}
	return checkingBit;
}

byte *ConvSelfOrthCoder::Decode(byte* src)
{
	BinaryMatrix *encodedVector = BinaryMatrix::CreateVectorFromBinaryData(src, 2);
	bool incomingEncodedBit = encodedVector->GetItem(0, 0);
	bool incomingCheckingBit = encodedVector->GetItem(0, 1);

	bool checkingBitFromCoderForDecoder;
	bool encodedBitFromCoderForDecoder;
	_coderForDecoder->EncodeCore(incomingEncodedBit, encodedBitFromCoderForDecoder, checkingBitFromCoderForDecoder);

	bool currentSyndrome = incomingCheckingBit ^ checkingBitFromCoderForDecoder;
	ShiftSyndromeRegistryRight();
	_syndromeRegistry->SetItem(0, 0, currentSyndrome);

	bool sumOfSyndromes = false;
	for (int i = 0; i < _syndromeRegistry->GetColCount(); i++)
	{
		sumOfSyndromes = sumOfSyndromes ^ _syndromeRegistry->GetItem(0, i);
	}
	bool thresholdConditionResult = CheckThresholdCondition(sumOfSyndromes);

	bool lastBitInRegistry = _coderRegistry->GetItem(0, _coderRegistry->GetColCount() - 1);
	bool decodedBit = thresholdConditionResult ^ lastBitInRegistry;

	bool *resultBits = new bool[1];
	resultBits[0] = decodedBit;

	int lengthOfDecodedDataByteArray;
	byte *decodedData = ByteUtil::StoreBoolArrayAsBytes(resultBits, 1, lengthOfDecodedDataByteArray);

	delete [] resultBits;

	return decodedData;
};

bool ConvSelfOrthCoder::CheckThresholdCondition(bool sumOfSyndromes)
{
	float t = (float)_syndromeRegistry->GetColCount() / 2;
	int countOfIdentities = 0;
	for (int i = 0; i < _syndromeRegistry->GetColCount(); i++) {
		if (_syndromeRegistry->GetItem(0, i) == true) {
			countOfIdentities++;
		}
	}
	return (float)countOfIdentities > t;
	//return sumOfSyndromes == false;
}

void ConvSelfOrthCoder::SetFirstItem(bool &val) {
	_coderRegistry->SetItem(0, 0, val);
}

void ConvSelfOrthCoder::ShiftCoderRegistryRight()
{
	ShiftRegistryRight(_coderRegistry);
}

void ConvSelfOrthCoder::ShiftSyndromeRegistryRight()
{
	ShiftRegistryRight(_syndromeRegistry);
}

void ConvSelfOrthCoder::ShiftRegistryRight(BinaryMatrix *registry)
{
	for (int i = registry->GetColCount() - 1; i > 0; i--) {
		registry->SetItem(0, i, registry->GetItem(0, i - 1));
	}
	registry->SetItem(0, 0, false);
}

void ConvSelfOrthCoder::Init()
{
	_coderRegistry = new BinaryMatrix(1, _polynomPower + 1);
	_syndromeRegistry = new BinaryMatrix(1, _polynomPower + 1);
}

void ConvSelfOrthCoder::InitH()
{
	_H = _Hdelta->ConcatWidth(_Hidentity);
}

void ConvSelfOrthCoder::InitHIdentity()
{
	_Hidentity = BinaryMatrix::CreateIdentityMatrix(_polynomPower);
}

void ConvSelfOrthCoder::InitHdelta()
{
	_Hdelta = new BinaryMatrix(_polynomPower, _polynomPower);
	for (int i = 0; i < _polynomPower; i++)
	{
		for (int j = i; j < _polynomPower; j++)
		{
			_Hdelta->SetItem(j, i, _polynomFactors->GetItem(0, j - i));
		}
	}
}

int ConvSelfOrthCoder::GetEncodedBitsCount() {
	return _n0;
}
}