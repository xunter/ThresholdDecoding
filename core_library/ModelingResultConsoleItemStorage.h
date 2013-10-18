#pragma once


#include "core_library.h"
#include "ModelingResultItemStorage.h"

namespace ThresholdDecoding {

class ModelingResultConsoleItemStorage : public ModelingResultItemStorage {	
public:
	void Store(ModelingResultItem *item);
	void Complete();
};
}



