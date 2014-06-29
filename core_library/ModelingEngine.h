#pragma once


#include "core_library.h"
#include "BaseClass.h"
#include "Coder.h"
#include "DataBlockGenerator.h"
#include "ModelingResultItem.h"
#include <vector>
#include <queue>

namespace ThresholdDecoding {

	
	class DataTransmissionChannel : public BaseClass {
	public:
		virtual byte *HandleData(byte *data, int lenData) = null;
	};

	class BinarySymmetricChannel : public DataTransmissionChannel {
	public:
		BinarySymmetricChannel(double p0) {
			_p0 = p0;
		};

		byte *HandleData(byte *data, int lenData) {
			return MakeNoise(data, lenData);
		};
	protected:
		byte *MakeNoise(byte *data, int lenData) {
			int dataLen = lenData;
			int bytesLen = ByteUtil::GetByteLenForDataLen(dataLen);
			byte *noisedData = ByteUtil::CopyData(data, bytesLen);
			
			double pNoisePercent = _p0 * 100;
			for (int i = 0; i < dataLen; i++) {
				float randPercent = ((float)rand())/((float)RAND_MAX) * 100.0f;
				bool needsNoise = pNoisePercent >= randPercent;
				if (needsNoise) {
					//printf("Making noise in bit=%u...\n", i);
					ByteUtil::InvertBitInByteData(noisedData, dataLen, i);
				}
			}

			return noisedData;
		};
	private: 
		double _p0;
	};

	class TotalSimulationResult {
	public:
		TotalSimulationResult() {
			Items = new std::vector<ModelingResultItem *>();
			Pb = 0;
			FailsCounter = 0;
			BitErrorCounter = 0;
			pNoise = 0;
			pBitResult = 0;
			pResult = 0;
			pBlock = 0;
			totalCountIterations = 0;
		};

		virtual ~TotalSimulationResult() {
			delete Items;
		};

		double Pb;
		std::vector<ModelingResultItem *> *Items;
		long FailsCounter;
		long BitErrorCounter;
		double pNoise;
		double pBitResult;
		double pResult;
		double pBlock;
		long totalCountIterations;
	};

class ModelingEngine : public BaseClass {
private:
	int _dataBlockLen;
	double _pNoise;
	double _noiseCounter;
	bool _debugMode;
	Coder *_coder;
	DataBlockGenerator *_generator;
	int _decoderTactsLatency;
	std::vector<ModelingResultItem *> *_items;
	std::queue<ModelingResultItem *> *_workingItemsQueue;
	long _counter;
	DataTransmissionChannel *_channel;
	bool _storeItems;
	std::ostringstream _oss;
	bool _useCoder;
public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator, double pNoise, int originalDataBitsLen);
	virtual ~ModelingEngine(void);

	bool IsCoderUsed();
	void SetDebugMode(bool debugMode);
	void Init();
	void SetStoreItems(bool store);

	TotalSimulationResult *SimulateTotal(long volume, int limitErrors);
	ModelingResultItem *SimulateIteration();
	long GetNoiseCount();
	void SetDecoderLatency(int latencyTacts);
	std::vector<ModelingResultItem *> *GetItems();
	
	void SetChannel(DataTransmissionChannel *channel);
	DataTransmissionChannel *GetChannel();


};
}