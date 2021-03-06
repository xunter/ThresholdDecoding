#pragma once


#include "core_library.h"
#include "BaseClass.h"
#include "BinaryData.h"

namespace ThresholdDecoding {

class ModelingResultItem : public BaseClass {
private:
	BinaryData *_srcBlock;
	BinaryData *_encodedBlock;
	BinaryData *_receivedBlock;
	BinaryData *_decodedBlock;
	int _originalDataLen;
	int _bitDiffCount;
public:
	ModelingResultItem(int orignalDataLen);
	virtual ~ModelingResultItem(void);
	
	bool noiseExists;

	void ModelingResultItem::SetBitDiffCount(int count);
	int ModelingResultItem::GetBitDiffCount();

	void ModelingResultItem::SetSourceBlock(BinaryData *src);
	void ModelingResultItem::SetEncodedBlock(BinaryData *encoded);		
	void ModelingResultItem::SetReceivedBlock(BinaryData *data);		
	void ModelingResultItem::SetDecodedBlock(BinaryData *data);

	BinaryData *GetSourceBlock() const;
	BinaryData *GetEncodedBlock() const;
	BinaryData *GetReceivedBlock() const;
	BinaryData *GetDecodedBlock() const;

	bool ModelingResultItem::IsResultEqualsOriginal();
};
}


