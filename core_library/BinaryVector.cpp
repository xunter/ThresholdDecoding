#include "StdAfx.h"
#include "BinaryVector.h"

namespace ThresholdDecoding {

BinaryVector::BinaryVector(int size) : BinaryMatrix(1, size)
{
}


BinaryVector::~BinaryVector(void)
{
}


	BinaryVector *BinaryVector::CropVector(int start, int count = -1) {
		int endIndex = count == -1 ? GetRowCount() - 1 : (start + count - 1);
		BinaryMatrix *croped = Crop(0, 0, start, endIndex);
	};



}