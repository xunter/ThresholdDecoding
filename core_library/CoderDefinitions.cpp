
#include "CoderDefinitions.h"

namespace ThresholdDecoding {

	int CoderDefinition::GetCountInputs() {
		int currInputNum = 0;
		for (int i = 0; i < itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &item = itemsCoderDefinition.at(i);
			if (item.i > currInputNum) {
				currInputNum = item.i;
			}
		}
		return currInputNum;
	};

	int CoderDefinition::GetCountCheckOutputs() {
		int currNum = 0;
		for (int i = 0; i < itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &item = itemsCoderDefinition.at(i);
			if (item.j > currNum) {
				currNum = item.j;
			}
		}
		return currNum;
	};

	int CoderDefinition::GetCountOutputs() {
		int countInputs = GetCountInputs();
		int countCheckOutputs = GetCountCheckOutputs();
		int countOutputs = countInputs + countCheckOutputs;
		return countOutputs;
	};

	int CoderDefinition::GetMaxM() {
		int maxM = 0;
		for (int i = 0; i < itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &item = itemsCoderDefinition.at(i);
			for (int j = 0; j < item.powersPolynom.size(); j++) {
				int power = item.powersPolynom.at(j);
				if (power > maxM) maxM = power;
			}
		}
		return maxM;
	};

	
	std::vector<CoderDefinitionItem> *CoderDefinition::FilterItemsByOutputBranchIndex(int indexOutputBranch) {
		std::vector<CoderDefinitionItem> *filteredItems = new std::vector<CoderDefinitionItem>();
		for (int i = 0; i < itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &item = itemsCoderDefinition.at(i);
			if (item.j == indexOutputBranch + 1) filteredItems->push_back(item);
		}
		return filteredItems;
	};
}