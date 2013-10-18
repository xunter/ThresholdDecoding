#include "StdAfx.h"
#include "BinaryMatrix.h"
#include "ByteUtil.h"


namespace ThresholdDecoding {

using namespace std;

int BinaryMatrix::GetColCount() { return _col; };
int BinaryMatrix::GetRowCount() { return _row; };
int BinaryMatrix::GetBitsLength() { return _row * _col; };

BinaryMatrix *BinaryMatrix::Crop(int rowStart, int rowEnd, int colStart, int colEnd) {
	int newRowCount = rowEnd - rowStart + 1;
	int newColCount = colEnd - colStart + 1;
	BinaryMatrix *matrix = new BinaryMatrix(newRowCount, newColCount);
	for (int i = rowStart; i <= rowEnd; i++) {
		for (int j = colStart; j <= colEnd; j++) {
			matrix->SetItem(i - rowStart, j - colStart, GetItem(i, j));
		}
	}
	return matrix;
};

bool BinaryMatrix::IsZero() {
	for (int i = 0; i < _row; i++)
		for (int j = 0; j < _col; j++)
			if (GetItem(i, j) == true) return false;
	return true;
};

bool BinaryMatrix::IsVector() {
	return _row == 1;
};

BinaryMatrix *BinaryMatrix::LoadFromByteArray(byte *data, int row, int col) {
	BinaryMatrix *matrix = new BinaryMatrix(row, col);
	int dataLen = matrix->GetBitsLength();
	int byteArrayLen = ByteUtil::GetByteLenForDataLen(dataLen);
	int arrIndex = 0;
	int bitCounter = 0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (bitCounter >= BYTE_BIT_LEN) {
				arrIndex++;
				bitCounter = 0;
			}
			byte tempByte = data[arrIndex];
			matrix->SetItem(i, j, ByteUtil::IsBitSettedInByte(tempByte, bitCounter));
		}
	}
	return matrix;
};

byte *BinaryMatrix::StoreAsByteArray() {
	int dataLen = GetBitsLength();
	int lengthOfByteArray;
	byte *arr = ByteUtil::StoreBoolArrayAsBytes(_matrixMemory, _lengthOfMatrixItems, lengthOfByteArray);
	return arr;
};

BinaryMatrix::BinaryMatrix(int rowSize, int colSize) {
	_row = rowSize;
	_col = colSize;

	_matrixMemory = null;

	InitMatrixArray();
};

BinaryMatrix::~BinaryMatrix() {
	free(_matrixMemory);
	/*
	for (int i = 0; i < _row; i++) {
		delete [] _matrixArr[i];
	}
	delete [] _matrixArr;
	*/
};

void BinaryMatrix::InitMatrixArray() {	
	_lengthOfMatrixItems = _row * _col;
	_matrixMemory = (bool *)malloc(sizeof(bool) * _row * _col);

	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < _col; j++) {
			SetItem(i, j, false);
		}
	}
};

void BinaryMatrix::SetItem(int row, int col, bool val) {
	if (row < 0 || col < 0 || row > _row - 1 || col > _col - 1) throw new std::exception("Index is out of the range!");
	_matrixMemory[row * _col + col] = val;
};

bool BinaryMatrix::GetItem(int row, int col) {
	if (row < 0 || col < 0 || row > _row - 1 || col > _col - 1) throw new std::exception("Index is out of the range!");
	return _matrixMemory[row * _col + col];
};

BinaryMatrix *BinaryMatrix::Transpose() {
	int tRowCount = _col;
	int tColCount = _row;
	BinaryMatrix *matrix = new BinaryMatrix(tRowCount, tColCount);
	for (int i = 0; i < tRowCount; i++)
		for (int j = 0; j < tColCount; j++) {
			bool item = GetItem(j, i);
			matrix->SetItem(i, j, item);
		}
	return matrix;
};

BinaryMatrix *BinaryMatrix::ConcatWidth(BinaryMatrix *other) {
	if (_row != other->GetRowCount()) return null;

	int resultColCount = _col + other->GetColCount();
	BinaryMatrix *matrix = new BinaryMatrix(_row, resultColCount);

	int edge = _col;
	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < resultColCount; j++) {
			if (j < edge) {
				matrix->SetItem(i, j, GetItem(i, j));
			} else {
				int otherCol = j - _col;
				matrix->SetItem(i, j, other->GetItem(i, otherCol));
			}
		}
	}

	return matrix;
};

BinaryMatrix *BinaryMatrix::ConcatHeight(BinaryMatrix *other) {
	if (_col != other->GetColCount()) return null;

	int resultRowCount = _row + other->GetRowCount();
	BinaryMatrix *matrix = new BinaryMatrix(resultRowCount, _col);

	int edge = _row;
	for (int i = 0; i < _col; i++) {
		for (int j = 0; j < resultRowCount; j++) {
			if (j < edge) {
				matrix->SetItem(j, i, GetItem(j, i));
			} else {
				int otherRow = j - _row;
				matrix->SetItem(j, i, other->GetItem(otherRow, i));
			}
		}
	}

	return matrix;
};

BinaryMatrix *BinaryMatrix::Mul(BinaryMatrix *other) {
	if (_col != other->GetRowCount()) return null;
	int resultMatrixRowCount = _row;
	int resultMatrixColCount = other->GetColCount();
	BinaryMatrix *matrix = new BinaryMatrix(resultMatrixRowCount, resultMatrixColCount);
	for (int i = 0; i < resultMatrixRowCount; i++) {
		for (int j = 0; j < resultMatrixColCount; j++) {				
			bool temp = false;
			for (int r = 0; r < _col; r++) {
				bool tempConjunction = GetItem(i, r) && other->GetItem(r, j);
				temp = Xor(temp, tempConjunction);
			}
			matrix->SetItem(i, j, temp);
		}
	}
	return matrix;
};

BinaryMatrix *BinaryMatrix::CreateIdentityMatrix(int size) {
	BinaryMatrix *matrix = new BinaryMatrix(size, size);
	for (int i = 0; i < size; i++) matrix->SetItem(i, i, true);
	return matrix;
};

BinaryMatrix *BinaryMatrix::CreateVector(int size) {
	BinaryMatrix *matrix = new BinaryMatrix(1, size);
	return matrix;	
};

BinaryMatrix *BinaryMatrix::CreateVectorFromBinaryData(byte *data, int bitLen) {
	BinaryMatrix *matrix = CreateVector(bitLen);
	int byteLen = ByteUtil::GetByteLenForDataLen(bitLen);

	for (int i = 0; i < byteLen; i++) {
		byte b = data[i];
		for (int j = 0; j < BYTE_BIT_LEN; j++) {
			int itemIndex = i * BYTE_BIT_LEN + j;
			if (itemIndex < bitLen) {
				byte bitByte = ByteUtil::GetOnlyBitByte(b, j);
				if (bitByte != 0) {
					matrix->SetItem(0, itemIndex, true);
				} 
			}
		}
	}
	return matrix;
};

bool BinaryMatrix::IsSubMatrixEquals(int rowStart, int rowEnd, int colStart, int colEnd, BinaryMatrix *other) {
	for (int i = rowStart; i <= rowEnd; i++)
		for (int j = colStart; j <= colEnd; j++)
			if (GetItem(i, j) != other->GetItem(i - rowStart, j - colStart)) return false;
	return true;
};

void BinaryMatrix::InvertItem(int row, int col) {
	this->SetItem(row, col, !GetItem(row, col));
};


void BinaryMatrix::SortColumnsAsc() {
	int itemsLen = _col;
	bool swapped = true;
	int n = 0;
	while (swapped) {
		swapped = false;
		for (int i = 0; i < itemsLen - n - 1; i++) {
			int j = i + 1;
			if (CompareColumns(i, j) > 0) {
				SwapColumns(i, j);
				swapped = true;
			}
			n++;
		}

	}
};

int BinaryMatrix::CompareColumns(int leftIndex, int rightIndex) {
	for (int i = 0; i < _row; i++) {
		bool leftItem = GetItem(i, leftIndex);
		bool rightItem = GetItem(i, rightIndex);
		if (rightItem != leftItem) {
			if (leftItem == true) {
				return 1;
			} else {
				return -1;
			}
		}
	}
	return 0;
};

void BinaryMatrix::SwapColumns(int leftIndex, int rightIndex) {
	for (int i = 0; i < _row; i++) {
		bool temp = GetItem(i, leftIndex);
		SetItem(i, leftIndex, GetItem(i, rightIndex));
		SetItem(i, rightIndex, temp);
	}
};

void BinaryMatrix::SortRowsAsc() {
	int itemsLen = _row;
	bool swapped = true;
	int n = 0;
	while (swapped) {
		swapped = false;
		int loopCount = itemsLen - n - 1;
		for (int i = 0; i < loopCount; i++) {
			int j = i + 1;
			if (CompareRows(i, j) > 0) {
				SwapRows(i, j);
				swapped = true;
			}
		}
		n++;
	}
};

int BinaryMatrix::CompareRows(int leftIndex, int rightIndex) {
	for (int i = 0; i < _col; i++) {
		bool leftItem = GetItem(leftIndex, i);
		bool rightItem = GetItem(rightIndex, i);
		if (rightItem != leftItem) {
			if (leftItem == true) {
				return 1;
			} else {
				return -1;
			}
		}
	}
	return 0;
};

void BinaryMatrix::SwapRows(int leftIndex, int rightIndex) {
	for (int i = 0; i < _col; i++) {
		bool temp = GetItem(leftIndex, i);
		SetItem(leftIndex, i, GetItem(rightIndex, i));
		SetItem(rightIndex, i, temp);
	}
};

bool BinaryMatrix::Xor(bool &left, bool &right) {
	return left != right;
};

BinaryMatrix *BinaryMatrix::Copy() {
	BinaryMatrix *matrix = new BinaryMatrix(_row, _col);
	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < _col; j++) {
			matrix->SetItem(i, j, GetItem(i, j));
		}
	}
	return matrix;
};

void BinaryMatrix::DisplayConsole(char *name) {
	cout << name << "[" << _row << "x" << _col << "]:" << endl;
	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < _col; j++) {
			cout << " " << (GetItem(i, j) ? "1" : "0");
		}
		cout << endl;
	}
};
}