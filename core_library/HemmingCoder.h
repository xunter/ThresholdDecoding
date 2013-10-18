#pragma once

#include "core_library.h"
#include "coder.h"
#include "BinaryMatrix.h"

namespace ThresholdDecoding {

class HemmingCoder : public Coder {
private:
	int _dataBlockLen, _entireBlockLen, _additionalBlockLen;	
	BinaryMatrix *_generatingMatrix;
	BinaryMatrix *_checkingMatrix;
	BinaryMatrix *_checkingMatrixSorted;
	BinaryMatrix *_identityGeneratingMatrix;
	BinaryMatrix *_identityCheckingMatrix;
	BinaryMatrix *_pMatrix;
		
	void InitIdentityGeneratingMatrix();
	void InitIdentityCheckingMatrix();
	void InitPMatrix();
	void InitGeneratingMatrix();
	void InitCheckingMatrix();

	

public:
	HemmingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen);
	~HemmingCoder();
	void Init();	
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
	int GetEncodedBitsCount();
};
}