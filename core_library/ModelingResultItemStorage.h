#pragma once


#include "core_library.h"
#include "ModelingResultItem.h"

namespace ThresholdDecoding {

class ModelingResultItemStorage : public BaseClass {
public:
	virtual void Store(ModelingResultItem *item) = null;
	virtual void Complete() = null;
};
}

