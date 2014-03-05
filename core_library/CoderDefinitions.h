#pragma once

#include "core_library.h"

namespace ThresholdDecoding {

struct CoderDefinitionItem {
	int i;
	int j;
	int countBranches;
	std::vector<int> powersPolynom;
};

struct CoderDefinition {
public:
	int k0;
	int n0;
	float R;
	std::vector<CoderDefinitionItem> itemsCoderDefinition;
	int GetCountInputs();
	int GetCountCheckOutputs();
	int GetCountOutputs();
	int GetMaxM();
	std::vector<CoderDefinitionItem> *FilterItemsByOutputBranchIndex(int indexOutputBranch);
};

}