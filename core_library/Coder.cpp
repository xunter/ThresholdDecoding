#include "StdAfx.h"
#include "Coder.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	int Coder::GetEncodedBytesCount() {
		return ByteUtil::GetByteLenForDataLen(GetEncodedBitsCount());
	}

	void Coder::DisplayDebugInfo(const char *label) {
		
	};
}

