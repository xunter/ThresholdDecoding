#include "core_library.h"
#include "ByteUtil.h"
#include "Coder.h"
#include "DataBlockGenerator.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"
#include <windows.h>

#define ME_VERBOSE_LOGGING 0
#define ME_PROGRESS_CODE_ENABLED 0

namespace ThresholdDecoding {


ModelingEngine::ModelingEngine(Coder *coder, DataBlockGenerator *generator, double pNoise, int originalDataBitsLen) {
	_coder = NULL;
	_useCoder = false;
	_debugMode = false;
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

void ModelingEngine::SetDebugMode(bool debugMode) {
	_debugMode = debugMode;
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

TotalSimulationResult *ModelingEngine::SimulateTotal(long volume, int limitErrors) {
	TotalSimulationResult *totalResult = new TotalSimulationResult();

	double pResult = 0;
	long bitFailsCounter = 0;
	long failsCounter = 0;
	long bitErrorCounter = 0;
	long mCounter = volume;
	long mMax = limitErrors;
	long loopCounter = 0;
	long v = 0;
	int m = _decoderTactsLatency;
	double donePercentage = 0.00;

	while (mCounter < mMax) {
		double currDonePercentage = 100.00 * ((double)mCounter / mMax);
		if (currDonePercentage - donePercentage >= 1) {
			donePercentage = currDonePercentage;
			std::cout << "Done: " << std::fixed << std::setprecision(1) << donePercentage << " % " << "countIterations=" << std::setbase(10) << v << " countErrorsBit=" << bitFailsCounter << std::endl;
		}	
		
#if _DEBUG
		if (_debugMode) system("cls");
#endif
		//try {
			ModelingResultItem *item = null;
			//try {
				item = SimulateIteration();
			//} catch (...) {
				//std::cout << "Failed to simulate iteration! Current iteration = " << std::fixed << std::setbase(10) << loopCounter << std::endl;
				//break;
				//throw;
			//}
		if (item != null && loopCounter >= m) {
			int bitDiffCount = item->GetBitDiffCount();
			if (!item->IsResultEqualsOriginal()) { 
				bitFailsCounter += bitDiffCount;
				mCounter++;
				failsCounter++;
			}
			bitErrorCounter += bitDiffCount;
			v++;
			/*
			if (v % (long)1e+7 == 0) {
#if _DEBUG
				std::cout << "Clearing the app memory... v = " << std::fixed << std::setbase(10) << v << std::endl;
#endif
				HANDLE currHandle = GetCurrentProcess();
				SetProcessWorkingSetSize(currHandle, 0xFFFFFFFF, 0xFFFFFFFF);
			}
			*/
		}
		if (!_storeItems && item != null) { 
			delete item;
		};
		loopCounter++;
		//} catch (...) {
			//std::cout << "ModelingEngine::SimulateTotal error!" << std::endl;
			//throw;
		//}
#if _DEBUG
		if (_debugMode) {
			_coder->DisplayDebugInfo("global");
			std::cout << "N iteration = " << loopCounter << std::endl;
			system("pause");
		}
#endif
	}
		
	double pNoise = (double)GetNoiseCount() / (double)v;

	/*
	Вероятность искажения бита.
	Количество искаженных битов / общее количество переданных бит
	*/
	double pBitResult = (double)bitFailsCounter / (double)(v * _dataBlockLen);

	pResult = (double)failsCounter / (double)v;
	
	/*
	Вероятность искажения блока
	1 - (1 - вероятность искажения бита) ^ общее число битов на блок
	*/
	double pBlock = 1 - pow((1 - pBitResult), _dataBlockLen);

	totalResult->totalCountIterations = loopCounter;

	totalResult->pNoise = pNoise;
	totalResult->pBitResult = pBitResult;
	totalResult->pResult = pResult;

	totalResult->pBlock = pBlock;
	
	if (_storeItems) {
		totalResult->Items->reserve(_items->size());
		for (int i = 0; i < _items->size(); i++) totalResult->Items->push_back(_items->at(i));
	}

	return totalResult;
};

void ShowDataConsole(byte *data, int lengthBits, char *labelText) {	
	std::cout << labelText << ": ";
	ByteUtil::ShowDataBlockOnConsole(data, lengthBits);
};

ModelingResultItem *ModelingEngine::SimulateIteration() {
#if ME_PROGRESS_CODE_ENABLED
	int progressCode = 0;
#endif
#if ME_VERBOSE_LOGGING
	std::ostringstream oss;
#endif
	
	ModelingResultItem *item = null;
	//try {
#if ME_VERBOSE_LOGGING
		oss << "Simulating..." << std::endl;
#endif
		int dataBitsCount = _generator->GetDataBlockLength();

		int bytesLen = ByteUtil::GetByteLenForDataLen(dataBitsCount);
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 1;
#endif
#if ME_VERBOSE_LOGGING
		oss << "bytesLen=" << bytesLen << ", dataBitsCount=" << dataBitsCount << ". Creating item..." << std::endl;
#endif
		item = new ModelingResultItem(dataBitsCount);
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 2;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Pushing item into the items queue..." << std::endl;
#endif
		_workingItemsQueue->push(item);
		if (_storeItems) _items->push_back(item);
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 3;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Generating source data..." << std::endl;
#endif

		byte *originalData = null;
		originalData = _generator->GenerateBlock();

#if ME_PROGRESS_CODE_ENABLED
		progressCode = 4;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Wrapping source data into an BinaryData object..." << std::endl;
#endif		
		BinaryData *sourceData = new BinaryData(originalData, bytesLen, dataBitsCount);
#if _DEBUG
		if (_debugMode) sourceData->ShowBitsOnOutput(std::cout, "Source data");
#endif

#if ME_PROGRESS_CODE_ENABLED
		progressCode = 5;
#endif
		
#if ME_VERBOSE_LOGGING
		oss << "Setting the source data withing the item..." << std::endl;
#endif
		item->SetSourceBlock(sourceData);
	
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 6;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Encoding the source data using the coder..." << std::endl;
#endif

		byte *transferData = originalData;
		int countTransferDataBits = dataBitsCount;
		int countTransferBytes = bytesLen;

		if (_useCoder) {
			
			byte *encodedData = null;
			encodedData = _coder->Encode(originalData);

			int countEncodedBits = _coder->GetEncodedBitsCount();
			int countEncodedBytes = _coder->GetEncodedBytesCount();

			countTransferDataBits = countEncodedBits;
			countTransferBytes = countTransferBytes;
			transferData = encodedData;

	#if ME_PROGRESS_CODE_ENABLED
			progressCode = 7;
	#endif

	#if ME_VERBOSE_LOGGING
			oss << "countEncodedBits=" << countEncodedBits << " countEncodedBytes=" << countEncodedBytes << std::endl;
			oss << "Wrapping encoded data as a BinaryData object..." << std::endl;
	#endif
			BinaryData *encodedBinData = new BinaryData(encodedData, countEncodedBytes, countEncodedBits);
	#if _DEBUG
			if (_debugMode) encodedBinData->ShowBitsOnOutput(std::cout, "Encoded data");
	#endif

	#if ME_PROGRESS_CODE_ENABLED
			progressCode = 8;
	#endif

	#if ME_VERBOSE_LOGGING
			oss << "Setting encoded block within the item..." << std::endl;
	#endif
			item->SetEncodedBlock(encodedBinData);
	
	#if ME_PROGRESS_CODE_ENABLED
			progressCode = 9;
	#endif
		}
#if ME_VERBOSE_LOGGING
		oss << "Making noise for encoded data..." << std::endl;
#endif
		byte *noisedData = _channel->HandleData(transferData, countTransferDataBits);

#if ME_PROGRESS_CODE_ENABLED
		progressCode = 10;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Wrapping noised data as a BinaryData object..." << std::endl;
#endif
		BinaryData *receivedBinData = new BinaryData(noisedData, countTransferBytes, countTransferDataBits);
#if _DEBUG
		if (_debugMode) receivedBinData->ShowBitsOnOutput(std::cout, "Received data");
#endif

#if ME_PROGRESS_CODE_ENABLED
		progressCode = 11;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Setting the noised data within the item as the received data...";
#endif
		item->SetReceivedBlock(receivedBinData);
	
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 12;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Checking encoded and received data equality..." << std::endl;
#endif
		bool equal = ByteUtil::IsDataEqual(transferData, noisedData, countTransferDataBits);
		if (!equal) {
#if ME_VERBOSE_LOGGING
			oss << "Encoded and received data are not equal! Noise counter=" << _noiseCounter  << std::endl;
#endif
#if _DEBUG
			if (_debugMode) std::cout << "Noise exists" << std::endl;
#endif
			item->noiseExists = true;
			_noiseCounter++;
		}
	
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 13;
#endif
		byte *destData = noisedData;

		if (_useCoder) {
#if ME_VERBOSE_LOGGING
		oss << "Decoding received data..." << std::endl;
#endif
		byte *decodedData = _coder->Decode(noisedData);
		destData = decodedData;
	
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 14;
#endif
		/**/
		/*
#if ME_VERBOSE_LOGGING
		oss << "Wrapping the decoded data as a BinaryData object..." << std::endl;
#endif

		BinaryData *decodedBinData = new BinaryData(decodedData, bytesLen, dataBitsCount);
#if _DEBUG
		if (_debugMode) decodedBinData->ShowBitsOnOutput(std::cout, "Decoded data");
#endif

		progressCode = 15;
#if ME_VERBOSE_LOGGING
		oss << "Setting the decoded data within the item..." << std::endl;
#endif
		item->SetDecodedBlock(decodedBinData);
		*/
		}
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 16;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Checking latency... Latency=" << _decoderTactsLatency << " Counter=" << std::fixed << _counter << std::endl;
#endif
		item = null;
		if (_counter >= _decoderTactsLatency) {
#if ME_VERBOSE_LOGGING
			oss << "Latency is over. Dequeuing the next item from the queue..." << std::endl;
#endif
			ModelingResultItem *waitingItem = _workingItemsQueue->front();
#if ME_VERBOSE_LOGGING
			oss << "Removing last item from the queue..." << std::endl;
#endif
			_workingItemsQueue->pop();

#if ME_PROGRESS_CODE_ENABLED
			progressCode = 17;
#endif

			if (_useCoder) {
	#if ME_VERBOSE_LOGGING
				oss << "Setting decoded data within the item..." << std::endl;
	#endif
				BinaryData *prevDecodedBlock = waitingItem->GetDecodedBlock();
				if (prevDecodedBlock != NULL) 
					delete prevDecodedBlock;

				waitingItem->SetDecodedBlock(new BinaryData(destData, bytesLen, dataBitsCount));
			}
			bool noiseExists = waitingItem->noiseExists;

#if ME_PROGRESS_CODE_ENABLED
			progressCode = 18;
#endif

#if ME_VERBOSE_LOGGING
			oss << "Checking noise... Noise exists: " << noiseExists << std::endl;
#endif

#if ME_PROGRESS_CODE_ENABLED
			progressCode = 19;
#endif

#if ME_VERBOSE_LOGGING
			oss << "Computing bit diff count..." << std::endl;
#endif
			int bitDiffCount = ByteUtil::ComputeBitDiff(waitingItem->GetSourceBlock()->GetData(), destData, dataBitsCount);

#if ME_PROGRESS_CODE_ENABLED
			progressCode = 20;
#endif

#if ME_VERBOSE_LOGGING
			oss << "Bit diff count=" << bitDiffCount << std::endl;
#endif
#if _DEBUG
			if (bitDiffCount > 0) {		

				ShowDataConsole(waitingItem->GetSourceBlock()->GetData(), dataBitsCount, "Original data");
				if (_useCoder) ShowDataConsole(waitingItem->GetEncodedBlock()->GetData(), countTransferDataBits, "Encoded data");
				ShowDataConsole(waitingItem->GetReceivedBlock()->GetData(), countTransferDataBits, "Received data");
				if (_useCoder) ShowDataConsole(waitingItem->GetDecodedBlock()->GetData(), dataBitsCount, "Decoded data");
			
			}
#endif

#if ME_PROGRESS_CODE_ENABLED
			progressCode = 21;
#endif
			
#if ME_VERBOSE_LOGGING
			oss << "Setting bit diff count within the item..." << std::endl;
#endif
			waitingItem->SetBitDiffCount(bitDiffCount);
			item = waitingItem;
		} else {
			delete destData;
		}
	
#if ME_PROGRESS_CODE_ENABLED
		progressCode = 22;
#endif

#if ME_VERBOSE_LOGGING
		oss << "Incrementing the counter..." << std::endl;
#endif
		_counter++;

#if ME_PROGRESS_CODE_ENABLED
		progressCode = 23;
#endif
#if ME_VERBOSE_LOGGING
		oss << "Returning the item...";
#endif
		

	//} catch (...) {
	//std::cout << "ModelingEngine::SimulateIteration failed on progress=" << progressCode << std::endl;
#if ME_VERBOSE_LOGGING
	const std::string progressMsgStr = oss.str();
	std::cout << progressMsgStr << std::endl;
#endif
		//throw;
	//}
	
	return item;
};

long ModelingEngine::GetNoiseCount()
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
	_useCoder = _coder != NULL;
};

bool ModelingEngine::IsCoderUsed() {
	return _useCoder;
};

}