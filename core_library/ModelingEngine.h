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
		BinarySymmetricChannel(float p0) {
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
			
			float pNoisePercent = _p0 * 100;
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
		float _p0;
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

		float Pb;
		std::vector<ModelingResultItem *> *Items;
		int FailsCounter;
		int BitErrorCounter;
		float pNoise;
		float pBitResult;
		float pResult;
		float pBlock;
		int totalCountIterations;
	};

class ModelingEngine : public BaseClass {
private:
	int _dataBlockLen;
	float _pNoise;
	float _noiseCounter;

	Coder *_coder;
	DataBlockGenerator *_generator;
	int _decoderTactsLatency;
	std::vector<ModelingResultItem *> *_items;
	std::queue<ModelingResultItem *> *_workingItemsQueue;
	int _counter;
	DataTransmissionChannel *_channel;
	bool _storeItems;
	
public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen);
	virtual ~ModelingEngine(void);
	void Init();
	void SetStoreItems(bool store);

	TotalSimulationResult *SimulateTotal(int volume, int limitErrors);
	ModelingResultItem *SimulateIteration();
	int GetNoiseCount();
	void SetDecoderLatency(int latencyTacts);
	std::vector<ModelingResultItem *> *GetItems();
	
	void SetChannel(DataTransmissionChannel *channel);
	DataTransmissionChannel *GetChannel();


};
}