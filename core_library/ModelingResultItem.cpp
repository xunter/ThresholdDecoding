#include "StdAfx.h"
#include "ModelingResultItem.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	ModelingResultItem::~ModelingResultItem(void) {
		if (_srcBlock != NULL) delete _srcBlock;
		if (_encodedBlock != NULL) delete _encodedBlock;
		if (_receivedBlock != NULL) delete _receivedBlock;
		if (_decodedBlock != NULL) delete _decodedBlock;
	}

	void ModelingResultItem::SetSourceBlock(BinaryData *src) {
		_srcBlock = src;
	};
		
	void ModelingResultItem::SetEncodedBlock(BinaryData *encoded) {
		_encodedBlock = encoded;
	};
		
	void ModelingResultItem::SetReceivedBlock(BinaryData *data) {
		_receivedBlock = data;
	};
		
	void ModelingResultItem::SetDecodedBlock(BinaryData *data) {
		_decodedBlock = data;
	};

	bool ModelingResultItem::IsResultEqualsOriginal() {
		return _bitDiffCount == 0;
	};

	ModelingResultItem::ModelingResultItem(int originalDataLen) {
		noiseExists = false;
		_originalDataLen = originalDataLen;

		_srcBlock = null;
		_encodedBlock = null;
		_receivedBlock = null;
		_decodedBlock = null;
	};

	void ModelingResultItem::SetBitDiffCount(int count) {
		_bitDiffCount = count;
	}

	int ModelingResultItem::GetBitDiffCount() {
		return _bitDiffCount;
	}


	BinaryData *ModelingResultItem::GetSourceBlock() const {
		return _srcBlock;	
	};

	BinaryData *ModelingResultItem::GetEncodedBlock() const {
		return _encodedBlock;
	};
	BinaryData *ModelingResultItem::GetReceivedBlock() const {
		return _receivedBlock;
	};
	BinaryData *ModelingResultItem::GetDecodedBlock() const {
		return _decodedBlock;
	};

}