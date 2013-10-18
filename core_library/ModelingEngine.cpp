#include "core_library.h"
#include "ByteUtil.h"
#include "Coder.h"
#include "DataBlockGenerator.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"

namespace ThresholdDecoding {

ModelingEngine::ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen) {
	_dataBlockLen = originalDataBitsLen;
	_pNoise = pNoise;
	_coder = coder;
	_generator = generator;
	_items = new std::vector<ModelingResultItem *>();
	_decoderTactsLatency = 0;
	_counter = 0;
}

ModelingEngine::~ModelingEngine(void) {
	delete _items;
};

ModelingResultItem *ModelingEngine::Simulate() {	
	int dataBitsCount = _generator->GetDataBlockLength();

	int dataBlockBytesLen = _generator->GetDataBlockBytesLength();
	int bytesLen = ByteUtil::GetByteLenForDataLen(dataBlockBytesLen);

	ModelingResultItem *item = new ModelingResultItem(dataBlockBytesLen);
	_items->push_back(item);
	byte *originalData = _generator->GenerateBlock();
	item->SetSourceBlock(new BinaryData(originalData, dataBlockBytesLen, dataBitsCount));



	byte *encodedData = _coder->Encode(originalData);
	item->SetEncodedBlock(new BinaryData(encodedData, _coder->GetEncodedBytesCount(), _coder->GetEncodedBitsCount()));

	byte *noisedData = MakeNoise(encodedData);
	item->SetReceivedBlock(new BinaryData(noisedData, _coder->GetEncodedBytesCount(), _coder->GetEncodedBitsCount()));

	bool equal = ByteUtil::IsDataEqual(encodedData, noisedData, bytesLen, dataBlockBytesLen);
	if (!equal) {
		_noiseCounter++;
	}


	byte *decodedData = _coder->Decode(noisedData);

	if (_counter >= _decoderTactsLatency) {
		ModelingResultItem *waitingItem = _items->at(_counter - _decoderTactsLatency);
		waitingItem->SetDecodedBlock(new BinaryData(decodedData, dataBlockBytesLen, dataBitsCount));

		int bitDiffCount = ByteUtil::ComputeBitDiff(waitingItem->GetSourceBlock()->GetData(), decodedData, dataBlockBytesLen);
		waitingItem->SetBitDiffCount(bitDiffCount);
		
		item = waitingItem;
	}
	
	_counter++;
	return item;
}

byte *ModelingEngine::MakeNoise(byte *data) {
	int dataLen = _dataBlockLen;
	int bytesLen = ByteUtil::GetByteLenForDataLen(dataLen);
	byte *noisedData = ByteUtil::CopyData(data, bytesLen);

	float pNoisePercent = _pNoise * 100;

	int bitsCounter = 0;
	for (int i = 0; i < bytesLen; i++) {
		byte tempByte = noisedData[i];
		byte originalByte = tempByte;
		bool breakOuterLoop = false;
		for (int j = 0; j < BYTE_BIT_LEN; j++) {
			
			float randPercent = ((float)rand())/((float)RAND_MAX) * 100.0f;
			bool needsNoise =  pNoisePercent >= randPercent;
			if (needsNoise) {
				printf("Making noise in bit=%u...\n", bitsCounter);
				byte zeroByte = 0x00;
				byte leftPart = (0xff << (BYTE_BIT_LEN - j + 1) ) & tempByte;
				byte rightPart = (0xff >> (j + 1) ) & tempByte;
				byte bitByte = ByteUtil::GetOnlyBitByte(tempByte, j);
				tempByte = zeroByte | leftPart | rightPart;
				if (bitByte == 0x00) {
					byte ffByte = 0xff;
					tempByte |= ByteUtil::GetOnlyBitByte(ffByte, j);
				}
			}
			
			if (++bitsCounter >= dataLen) {
				breakOuterLoop = true;
				break;
			}
		}
		noisedData[i] = tempByte;
		if (breakOuterLoop) {
			break;
		}
	}

	return noisedData;
};

int ModelingEngine::GetNoiseCount()
{
	return _noiseCounter;
}

void ModelingEngine::SetDecoderLatency(int latencyTacts) {
	_decoderTactsLatency = latencyTacts;
};

std::vector<ModelingResultItem *> *ModelingEngine::GetItems() {
	return _items;
};
}