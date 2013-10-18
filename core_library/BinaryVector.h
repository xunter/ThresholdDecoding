#pragma once


#include "core_library.h"
#include "binarymatrix.h"

namespace ThresholdDecoding {
class BinaryVector :
	public BinaryMatrix
{
public:
	BinaryVector(int size);
	virtual ~BinaryVector(void);

	BinaryVector *CropVector(int start, int end = -1);
};
}