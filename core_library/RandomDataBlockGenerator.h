#pragma once
#include "datablockgenerator.h"

namespace ThresholdDecoding {

class RandomDataBlockGenerator : public DataBlockGenerator {
public:
	RandomDataBlockGenerator(int dataBlockLen);
	byte *GenerateBlock();
};
}
