#include "StdAfx.h"
#include "RandomDataBlockGenerator.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

RandomDataBlockGenerator::RandomDataBlockGenerator(int dataBlockLen) : DataBlockGenerator(dataBlockLen) {};

byte *RandomDataBlockGenerator::GenerateBlock() {
	int len = GetDataBlockLength();
	int byteLen = ByteUtil::GetByteLenForDataLen(len);
		
	byte *arr = new byte[byteLen];
	
	for (int i = 0; i < len; i++) {
		arr[i] = 0x00;
		if ((rand() % 100) % 2 == 0) {
			int indexByte = floor((float)i / BYTE_BIT_LEN);
			int indexBitInByte = i % BYTE_BIT_LEN;
			byte &b = arr[indexByte];
			ByteUtil::InvertBit(b, indexBitInByte);
		}
	}

	return arr;
}
}