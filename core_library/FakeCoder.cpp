#include "StdAfx.h"
#include "FakeCoder.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

FakeCoder::FakeCoder(int originalDataLen, int additionalBitsLen) {
	_originalDataLen = originalDataLen;
	_additionalBitsLen = additionalBitsLen;
};

byte *FakeCoder::Encode(byte *data) {
	int additionalBytesLen = ByteUtil::GetByteLenForDataLen(_additionalBitsLen);
	int encodedArrLen = _originalDataLen + additionalBytesLen;

	byte *encodedData = new byte[encodedArrLen];
	for (int i = 0; i < encodedArrLen; i++) {
		if (i + 1 < _originalDataLen) {
			encodedData[i] = data[i];
		} else {
			encodedData[i] = ByteUtil::GenerateByte();
		}
	}
	return encodedData;
};

byte *FakeCoder::Decode(byte *data) {
	int additionalBytesLen = ByteUtil::GetByteLenForDataLen(_additionalBitsLen);
	int decodedArrLen = _originalDataLen - additionalBytesLen;

	byte *decodedData = new byte[decodedArrLen];
	for (int i = 0; i < decodedArrLen; i++) {
		decodedData[i] = data[i];
	}
	return decodedData;	
};

int FakeCoder::GetEncodedBitsCount() {
	return _originalDataLen + _additionalBitsLen;
};
}