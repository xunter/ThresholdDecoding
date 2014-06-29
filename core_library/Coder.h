#pragma once

#include "core_library.h"
#include "baseclass.h"

namespace ThresholdDecoding {

class Coder : public BaseClass {
public:
	virtual byte *Encode(byte *src) = null;
	virtual byte *Decode(byte *src) = null;
	
	virtual int GetEncodedBitsCount() = null;
	virtual int GetEncodedBytesCount();
	virtual void Init() = null;
	virtual void DisplayDebugInfo(const char *label);
};
}

