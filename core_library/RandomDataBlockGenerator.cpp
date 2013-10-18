#include "StdAfx.h"
#include "RandomDataBlockGenerator.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

RandomDataBlockGenerator::RandomDataBlockGenerator(int dataBlockLen) : DataBlockGenerator(dataBlockLen) {};

byte *RandomDataBlockGenerator::GenerateBlock() {
	int len = _dataBlockLen;
	int byteLen = ByteUtil::GetByteLenForDataLen(len);
	byte *arr = new byte[byteLen];
	int dataBitCounter = 0;
	for (int i = 0; i < byteLen; i++) {
		byte c = ByteUtil::GenerateByte();
		dataBitCounter += BYTE_BIT_LEN;
		if (dataBitCounter > len) {
			int diff = dataBitCounter - len;
			int dataBitsLen = BYTE_BIT_LEN - diff;
			c = ByteUtil::FillDataInByte(c, dataBitsLen);
		}
		arr[i] = c;
	}		
	return arr;
}
}