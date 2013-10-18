#include "StdAfx.h"
#include "PMatrixColGenerator.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

BinaryMatrix *PMatrixColGenerator::ConvertNumToBinaryVector(int num) {
	BinaryMatrix *matrix = new BinaryMatrix(_rowSize, 1);
	for (int i = 0; i < _rowSize; i++) {
		byte b = (byte)_currNum;
		matrix->SetItem(i, 0, ByteUtil::IsBitSettedInByte(b, BYTE_BIT_LEN - _rowSize + i));
	}
	return matrix;
};

PMatrixColGenerator::PMatrixColGenerator(int rowSize) {
	_currNum = 0;
	_currTwoPower = 0;
	_rowSize = rowSize;
};

BinaryMatrix *PMatrixColGenerator::GetNextCol() {
	ComputeNextNum();
	return ConvertNumToBinaryVector(_currNum);
};

void PMatrixColGenerator::ComputeNextNum() {
	_currNum++;
	while (true) {
		int powerOfTwo = pow(2.0, _currTwoPower);
		if (_currNum < powerOfTwo) {
			break;
		} else if (_currNum == powerOfTwo) {
			_currNum++;			
			_currTwoPower++;
		}
	}
};
}