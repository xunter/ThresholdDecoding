#pragma once

#include "core_library.h"
#include "baseclass.h"
#include "BinaryMatrix.h"

namespace ThresholdDecoding {

	// It is a class helper to produce the P matrix for the Hemming coder.
	class PMatrixColGenerator : public BaseClass {
	private:
		int _rowSize;
		int _currNum;
		int _currTwoPower;

		void ComputeNextNum();
		BinaryMatrix *ConvertNumToBinaryVector(int num);
	public:
		PMatrixColGenerator(int rowSize);

		BinaryMatrix *GetNextCol();
	};

}

