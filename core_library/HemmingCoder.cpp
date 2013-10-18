#include "StdAfx.h"
#include "HemmingCoder.h"
#include "PMatrixColGenerator.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
HemmingCoder::HemmingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen) {
	_dataBlockLen = dataBlockLen;
	_entireBlockLen = entireBlockLen;
	_additionalBlockLen = additionalBlockLen;
}

HemmingCoder::~HemmingCoder() {
	BaseClass::Clean(_generatingMatrix);
	BaseClass::Clean(_checkingMatrix);
	BaseClass::Clean(_checkingMatrixSorted);
	BaseClass::Clean(_pMatrix);
	BaseClass::Clean(_identityGeneratingMatrix);
	BaseClass::Clean(_identityCheckingMatrix);
};

void HemmingCoder::Init() {
	InitIdentityGeneratingMatrix();
	InitIdentityCheckingMatrix();
	InitPMatrix();
	InitGeneratingMatrix();
	InitCheckingMatrix();
};

void HemmingCoder::InitIdentityCheckingMatrix() {
	_identityCheckingMatrix = BinaryMatrix::CreateIdentityMatrix(_additionalBlockLen);
	_identityCheckingMatrix->DisplayConsole("Hi");
};

void HemmingCoder::InitIdentityGeneratingMatrix() {
	_identityGeneratingMatrix = BinaryMatrix::CreateIdentityMatrix(_dataBlockLen);
	_identityGeneratingMatrix->DisplayConsole("Gi");
};

void HemmingCoder::InitCheckingMatrix() {	
	BinaryMatrix *pMatrixTranspose = _pMatrix->Transpose();
	pMatrixTranspose->DisplayConsole("Pt");
	_checkingMatrix = pMatrixTranspose->ConcatHeight( _identityCheckingMatrix );
	_checkingMatrix->DisplayConsole("H");
	_checkingMatrixSorted = _checkingMatrix->Copy();
	_checkingMatrixSorted->SortRowsAsc();
	_checkingMatrixSorted->DisplayConsole("Hs");
	BaseClass::Clean(pMatrixTranspose);
};

void HemmingCoder::InitGeneratingMatrix() {
	
	BinaryMatrix *pMatrixTranspose = _pMatrix->Transpose();
	_generatingMatrix = _identityGeneratingMatrix->ConcatWidth( pMatrixTranspose );
	_generatingMatrix->DisplayConsole("G");
	BaseClass::Clean(pMatrixTranspose);
};

void HemmingCoder::InitPMatrix() {
	PMatrixColGenerator pMatrixColGen(_additionalBlockLen);
	int pRowCount = _additionalBlockLen;
	int pColCount = _dataBlockLen;
	_pMatrix = new BinaryMatrix(pRowCount, pColCount);
	
	/*
	int maxNum = pow(2.0, _additionalBlockLen + 1);
	byte *numArr = new byte[_dataBlockLen];
	int numArrIndex = 0;
	for (byte j = 1; j <= maxNum; j++) {
		numArr[numArrIndex] = j;
		numArrIndex++;
		if (++numArrIndex >= _dataBlockLen) break;
	}
	*/
	
	int twoPower = 0;

	for (int j = 0; j < pColCount; j++) {		
		BinaryMatrix *pMatrixCol = pMatrixColGen.GetNextCol();
		for (int i = 0; i < pRowCount; i++) {
			_pMatrix->SetItem(i, j, pMatrixCol->GetItem(i, 0));
		}
		BaseClass::Clean(pMatrixCol);
	}

	_pMatrix->DisplayConsole("P");
};

byte *HemmingCoder::Encode(byte* src)
{
	BinaryMatrix *originalDataVector = BinaryMatrix::CreateVectorFromBinaryData(src, _dataBlockLen);
	originalDataVector->DisplayConsole("A");
	BinaryMatrix *encodedMatrix = originalDataVector->Mul(_generatingMatrix);

	encodedMatrix->DisplayConsole("E");
	byte *encodedData = encodedMatrix->StoreAsByteArray();

	BaseClass::Clean(originalDataVector);
	BaseClass::Clean(encodedMatrix);

	return encodedData;
}

byte *HemmingCoder::Decode(byte* src)
{
	BinaryMatrix *receivedVector = BinaryMatrix::CreateVectorFromBinaryData(src, _entireBlockLen);

	receivedVector->DisplayConsole("R");

	BinaryMatrix *syndromeVector = receivedVector->Mul(_checkingMatrixSorted);
	syndromeVector->DisplayConsole("S");
	bool errorOccurred = !syndromeVector->IsZero();

	if (errorOccurred) {
		byte bitIndex = 0x00;
		int colLen = syndromeVector->GetColCount();
		int syndromeBitIndexShift = BYTE_BIT_LEN - colLen;
		for (int i = 0; i < colLen; i++) {
			if (syndromeVector->GetItem(0, i) == true) {
				ByteUtil::SetBit(bitIndex, syndromeBitIndexShift + i);
			}
		}
		receivedVector->InvertItem(0, bitIndex - 1);

		/*
		for (int i = 0; i < _entireBlockLen; i++) {
			if (_checkingMatrix->IsSubMatrixEquals(i, i, 0, syndromeVector->GetColCount() - 1, syndromeVector))
			{
				receivedVector->InvertItem(0, i);
				break;
			}
		}
		*/
	};

	BinaryMatrix *decodedMatrix = receivedVector->Crop(0, 0, 0, _dataBlockLen - 1);
	decodedMatrix->DisplayConsole("D");
	byte *decodedData = decodedMatrix->StoreAsByteArray();

	BaseClass::Clean(receivedVector);
	BaseClass::Clean(syndromeVector);
	BaseClass::Clean(decodedMatrix);

	return decodedData;
};

int HemmingCoder::GetEncodedBitsCount() {
	return _dataBlockLen + _additionalBlockLen;
};
}