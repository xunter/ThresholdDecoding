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
	_noiseCounter = 0;
	_coder = coder;
	_generator = generator;
	_items = new std::vector<ModelingResultItem *>();
	_decoderTactsLatency = 0;
	_counter = 0;
}

ModelingEngine::~ModelingEngine(void) {
	delete _items;
};

void ModelingEngine::SetChannel(DataTransmissionChannel *channel) {
	_channel = channel;
};

DataTransmissionChannel *ModelingEngine::GetChannel() {
	return _channel;
};

TotalSimulationResult *ModelingEngine::SimulateTotal(int volume, int limitErrors) {
	TotalSimulationResult *totalResult = new TotalSimulationResult();
	std::vector<ModelingResultItem *> *modelingResultItems = new std::vector<ModelingResultItem *>();
	totalResult->Items = modelingResultItems;

	float pResult = 0;
	int bitFailsCounter = 0;
	int failsCounter = 0;
	int bitErrorCounter = 0;
	int mCounter = volume;
	int mMax = limitErrors;
	int loopCounter = 0;
	int v = 0;
	int m = _decoderTactsLatency;
	while (mCounter < mMax) {
		ModelingResultItem *item = SimulateIteration();
		if (loopCounter >= m) {
			if (!item->IsResultEqualsOriginal()) { 
				int diffCount = ByteUtil::ComputeBitDiff(item->GetSourceBlock()->GetData(), item->GetDecodedBlock()->GetData(), _dataBlockLen);
				bitFailsCounter += diffCount;
				mCounter++;
				failsCounter++;
			}
			bitErrorCounter += item->GetBitDiffCount();
			v++;
		}
		loopCounter++;
	}
		
	float pNoise = (float)GetNoiseCount() / (float)v;

	/*
	Вероятность искажения бита.
	Количество искаженных битов / общее количество переданных бит
	*/
	float pBitResult = (float)bitFailsCounter / (float)(v * _dataBlockLen);

	pResult = (float)failsCounter / (float)v;
	
	/*
	Вероятность искажения блока
	1 - (1 - вероятность искажения бита) ^ общее число битов на блок
	*/
	float pBlock = 1 - pow((1 - pBitResult), _dataBlockLen);

	totalResult->pNoise = pNoise;
	totalResult->pBitResult = pBitResult;
	totalResult->pResult = pResult;

	totalResult->pBlock = pBlock;

	return totalResult;
};

ModelingResultItem *ModelingEngine::SimulateIteration() {	

	int dataBitsCount = _generator->GetDataBlockLength();

	int bytesLen = ByteUtil::GetByteLenForDataLen(dataBitsCount);

	ModelingResultItem *item = new ModelingResultItem(dataBitsCount);
	_items->push_back(item);
	byte *originalData = _generator->GenerateBlock();
	item->SetSourceBlock(new BinaryData(originalData, bytesLen, dataBitsCount));
	
	byte *encodedData = _coder->Encode(originalData);
	int countEncodedBits = _coder->GetEncodedBitsCount();
	int countEncodedBytes = _coder->GetEncodedBytesCount();
	item->SetEncodedBlock(new BinaryData(encodedData, countEncodedBytes, countEncodedBits));
	
	byte *noisedData = _channel->HandleData(encodedData, countEncodedBits);
	item->SetReceivedBlock(new BinaryData(noisedData, countEncodedBytes, countEncodedBits));
	
	bool equal = ByteUtil::IsDataEqual(encodedData, noisedData, countEncodedBits);
	if (!equal) {
		item->noiseExists = true;
		_noiseCounter++;
	}


	byte *decodedData = _coder->Decode(noisedData);
	item->SetDecodedBlock(new BinaryData(decodedData, bytesLen, dataBitsCount));

	if (_counter >= _decoderTactsLatency) {
		ModelingResultItem *waitingItem = _items->at(_counter - _decoderTactsLatency);
		waitingItem->SetDecodedBlock(new BinaryData(decodedData, bytesLen, dataBitsCount));
		
		bool noiseExists = waitingItem->noiseExists;
		if (noiseExists) {
			std::cout << "Noise exists" << std::endl;
		}
	std::cout << "Original data: ";
	ByteUtil::ShowDataBlockOnConsole(waitingItem->GetSourceBlock()->GetData(), dataBitsCount);
	std::cout << "Encoded data: ";
	ByteUtil::ShowDataBlockOnConsole(waitingItem->GetEncodedBlock()->GetData(), countEncodedBits);
	std::cout << "Received data: ";
	ByteUtil::ShowDataBlockOnConsole(waitingItem->GetReceivedBlock()->GetData(), countEncodedBits);
	std::cout << "Decoded data: ";
	ByteUtil::ShowDataBlockOnConsole(decodedData, dataBitsCount);

		int bitDiffCount = ByteUtil::ComputeBitDiff(waitingItem->GetSourceBlock()->GetData(), decodedData, dataBitsCount);
		waitingItem->SetBitDiffCount(bitDiffCount);
		item = waitingItem;
	}
	
	_counter++;
	return item;
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

void ModelingEngine::Init() {
	
};

}