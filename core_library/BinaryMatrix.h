#pragma once

#include "baseclass.h"
#include "core_library.h"

namespace ThresholdDecoding {

class BinaryMatrix : public BaseClass {
private:
	bool *_matrixMemory;
	int _lengthOfMatrixItems;
	int _row;
	int _col;

	void InitMatrixArray();
	
	bool Xor(bool &left, bool &right);
public:
	BinaryMatrix(int rowSize, int colSize);
	~BinaryMatrix();

	int GetRowCount();
	int GetColCount();
	void SetItem(int row, int col, bool val);
	bool GetItem(int row, int col);
	void InvertItem(int row, int col);

	void SortColumnsAsc();	
	int CompareColumns(int leftIndex, int rightIndex);
	void SwapColumns(int leftIndex, int rightIndex);
	
	void SortRowsAsc();	
	int CompareRows(int leftIndex, int rightIndex);
	void SwapRows(int leftIndex, int rightIndex);

	byte *StoreAsByteArray();

	BinaryMatrix *Copy();
	BinaryMatrix *Transpose();
	BinaryMatrix *ConcatWidth(BinaryMatrix *other);
	BinaryMatrix *ConcatHeight(BinaryMatrix *other);
	BinaryMatrix *Mul(BinaryMatrix *other);
	BinaryMatrix *Crop(int rowStart, int rowEnd, int colStart, int colEnd);
	int GetBitsLength();
	bool IsVector();
	bool IsZero();

	void DisplayConsole(char *name);


	bool IsSubMatrixEquals(int rowStart, int rowEnd, int colStart, int colEnd, BinaryMatrix *other);
	
	static BinaryMatrix *CreateIdentityMatrix(int size);
	static BinaryMatrix *CreateVector(int size);
	static BinaryMatrix *CreateVectorFromBinaryData(byte *data, int bitLen);

	static BinaryMatrix *LoadFromByteArray(byte *data, int row, int col);
};
}