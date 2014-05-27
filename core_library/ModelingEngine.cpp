#include "core_library.h"
#include "ByteUtil.h"
#include "Coder.h"
#include "DataBlockGenerator.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"

namespace ThresholdDecoding {


ModelingEngine::ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen) {
	_storeItems = false;
	_dataBlockLen = originalDataBitsLen;
	_pNoise = pNoise;
	_noiseCounter = 0;
	_coder = coder;
	_generator = generator;
	_items = new std::vector<ModelingResultItem *>();
	_workingItemsQueue = new std::queue<ModelingResultItem *>();
	_decoderTactsLatency = 0;
	_counter = 0;
}

ModelingEngine::~ModelingEngine(void) {
	delete _items;
	delete _workingItemsQueue;
};

void ModelingEngine::SetStoreItems(bool store) {
	_storeItems = store;
};

void ModelingEngine::SetChannel(DataTransmissionChannel *channel) {
	_channel = channel;
};

DataTransmissionChannel *ModelingEngine::GetChannel() {
	return _channel;
};

TotalSimulationResult *ModelingEngine::SimulateTotal(int volume, int limitErrors) {
	TotalSimulationResult *totalResult = new TotalSimulationResult();

	float pResult = 0;
	int bitFailsCounter = 0;
	int failsCounter = 0;
	int bitErrorCounter = 0;
	int mCounter = volume;
	int mMax = limitErrors;
	int loopCounter = 0;
	int v = 0;
	int m = _decoderTactsLatency;
	double donePercentage = 0.00;
	while (mCounter < mMax) {
		double currDonePercentage = 100.00 * ((double)mCounter / mMax);
		if (currDonePercentage - donePercentage >= 1) {
			donePercentage = currDonePercentage;
			std::cout << "Done: " << std::fixed << std::setprecision(1) << donePercentage << " %" << std::endl;
		}
		ModelingResultItem *item = SimulateIteration();
		if (item != null && loopCounter >= m) {
			if (!item->IsResultEqualsOriginal()) { 
				int diffCount = item->GetBitDiffCount();
				bitFailsCounter += diffCount;
				mCounter++;
				failsCounter++;
			}
			bitErrorCounter += item->GetBitDiffCount();
			v++;
		}
		if (!_storeItems && item != null) delete item;
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

	totalResult->totalCountIterations = loopCounter;

	totalResult->pNoise = pNoise;
	totalResult->pBitResult = pBitResult;
	totalResult->pResult = pResult;

	totalResult->pBlock = pBlock;
	
	totalResult->Items->resize(_items->size(), null);
	for (int i = 0; i < _items->size(); i++) totalResult->Items->push_back(_items->at(i));

	return totalResult;
};

void ShowDataConsole(byte *data, int lengthBits, char *labelText) {	
	std::cout << labelText << ": ";
	ByteUtil::ShowDataBlockOnConsole(data, lengthBits);
};

ModelingResultItem *ModelingEngine::SimulateIteration() {	

	int dataBitsCount = _generator->GetDataBlockLength();

	int bytesLen = ByteUtil::GetByteLenForDataLen(dataBitsCount);

	ModelingResultItem *item = new ModelingResultItem(dataBitsCount);
	_workingItemsQueue->push(item);
	if (_storeItems) _items->push_back(item);
	
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

	item = null;
	if (_counter >= _decoderTactsLatency) {
		ModelingResultItem *waitingItem = _workingItemsQueue->front();
		_workingItemsQueue->pop();
		//waitingItem = _items->at(_counter - _decoderTactsLatency);
		//ModelingResultItem *waitingItem = _items->at(_counter - _decoderTactsLatency);
		waitingItem->SetDecodedBlock(new BinaryData(decodedData, bytesLen, dataBitsCount));
		
		bool noiseExists = waitingItem->noiseExists;
		if (noiseExists) {
			//std::cout << "Noise exists" << std::endl;
		}

		int bitDiffCount = ByteUtil::ComputeBitDiff(waitingItem->GetSourceBlock()->GetData(), decodedData, dataBitsCount);

		if (bitDiffCount > 0) {		
			ShowDataConsole(waitingItem->GetSourceBlock()->GetData(), dataBitsCount, "Original data");
			ShowDataConsole(waitingItem->GetEncodedBlock()->GetData(), countEncodedBits, "Encoded data");
			ShowDataConsole(waitingItem->GetReceivedBlock()->GetData(), countEncodedBits, "Received data");
			ShowDataConsole(decodedData, dataBitsCount, "Decoded data");
		}

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