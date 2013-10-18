#include "StdAfx.h"
#include "ModelingResultItem.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	ModelingResultItem::~ModelingResultItem(void) {
		BaseClass::Clean(_srcBlock);
		BaseClass::Clean(_encodedBlock);
		BaseClass::Clean(_receivedBlock);
		BaseClass::Clean(_decodedBlock);
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
		return ByteUtil::IsDataEqual(_srcBlock->GetData(), _decodedBlock->GetData(), _originalDataLen, _originalDataLen);
	};

	ModelingResultItem::ModelingResultItem(int originalDataLen) {
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


	BinaryData *ModelingResultItem::GetSourceBlock() {
		return _srcBlock;	
	};

	BinaryData *ModelingResultItem::GetEncodedBlock() {
		return _encodedBlock;
	};
	BinaryData *ModelingResultItem::GetReceivedBlock() {
		return _receivedBlock;
	};
	BinaryData *ModelingResultItem::GetDecodedBlock() {
		return _decodedBlock;
	};

}