#pragma once


#include "core_library.h"
#include "ModelingResultItem.h"

namespace ThresholdDecoding {

class ModelingResultItemStorage : public BaseClass {
public:
	virtual void Store(ModelingResultItem *item) = null;
	virtual void StoreBatch(std::vector<ModelingResultItem *> *batchItems) {
		for (int i = 0; i < batchItems->size(); i++) {
			Store(batchItems->at(i));
		}
	};
	virtual void Complete() = null;
};
}

