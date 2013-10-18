#pragma once


#include "core_library.h"
#include "BaseClass.h"
#include "Coder.h"
#include "DataBlockGenerator.h"
#include "ModelingResultItem.h"
#include <vector>

namespace ThresholdDecoding {

class ModelingEngine : public BaseClass {
private:
	int _dataBlockLen;
	float _pNoise;
	float _noiseCounter;

	Coder *_coder;
	DataBlockGenerator *_generator;
	int _decoderTactsLatency;
	std::vector<ModelingResultItem *> *_items;
	int _counter;
	
public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen);
	virtual ~ModelingEngine(void);
	ModelingResultItem *Simulate();
	byte *MakeNoise(byte *data);
	int GetNoiseCount();
	void SetDecoderLatency(int latencyTacts);
	std::vector<ModelingResultItem *> *GetItems();
};
}