#include "StdAfx.h"
#include "BinaryData.h"

namespace ThresholdDecoding {
	
	BinaryData::BinaryData(byte *binaryData, int length, int bitsCount) {		
		_binaryData = binaryData;
		_length = length;
		_bitsCount = bitsCount;
		_deleteData = true;
	}

	BinaryData::BinaryData(byte *data, int length)
	{
		_binaryData = data;
		_length = length;
		_bitsCount = length * BYTE_BIT_LEN;
		_deleteData = true;
	}
	
	BinaryData::~BinaryData(void)
	{
		if (_deleteData) {
			delete [] _binaryData;
		}
	}

	byte *BinaryData::GetData() {
		return _binaryData;
	}

	int BinaryData::GetDataLength() {
		return _length;
	}
	
	int BinaryData::GetBitsCount() {
		return _bitsCount;
	}
}