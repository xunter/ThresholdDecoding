#include "StdAfx.h"
#include "BinaryData.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	
	BinaryData::BinaryData(byte *binaryData, int length, int bitsCount) {		
		_binaryData = binaryData;
		_length = length;
		_bitsCount = bitsCount;
	};

	BinaryData::BinaryData(byte *data, int length)
	{
		_binaryData = data;
		_length = length;
		_bitsCount = length * BYTE_BIT_LEN;
	};
	
	BinaryData::~BinaryData(void)
	{
		delete [] _binaryData;
	};
	
	byte *BinaryData::GetData() const {
		return _binaryData;
	}

	int BinaryData::GetDataLength() {
		return _length;
	}
	
	int BinaryData::GetBitsCount() {
		return _bitsCount;
	};
	
	void BinaryData::ShowBitsOnOutput(std::ostream &os, const char *labelText) {
		os << labelText;
		if (strlen(labelText) > 0) os << ": ";
		for (int i = 0; i < _bitsCount; i++) {
			int indexByte = (int)floor((double)(i / BYTE_BIT_LEN));
			int indexBit = i % BYTE_BIT_LEN;
			const byte &b = _binaryData[indexByte];
			bool isBitSetted = ByteUtil::IsBitSettedInByte(b, indexBit);
			os << (isBitSetted ? "1" : "0");
		}
		os << std::endl;
	};
}