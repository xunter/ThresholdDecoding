
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
		FilterItemsByOutputBranchIndex(indexOutputBranch, *filteredItems);
		return filteredItems;
	};
	
	void CoderDefinition::FilterItemsByOutputBranchIndex(int indexOutputBranch, std::vector<CoderDefinitionItem> &vecItemsOutputBranch) {
		for (int i = 0; i < itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &item = itemsCoderDefinition.at(i);
			if (item.j == indexOutputBranch + 1) vecItemsOutputBranch.push_back(item);
		}
	};

	void CoderDefinition::GetItemsForDataBranchIndex(int indexData, std::vector<CoderDefinitionItem *> &vecItems) {
		for (int i = 0; i <  itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &coderDefItem = itemsCoderDefinition.at(i);
			if (coderDefItem.i - 1 == indexData) vecItems.push_back(&coderDefItem);
		}
	};

	CoderDefinitionItem *CoderDefinition::FindItemByDataCheckIndexes(int indexDataBranch, int indexCheckBranch) {		
		for (int i = 0; i <  itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem &coderDefItem = itemsCoderDefinition.at(i);
			if (coderDefItem.i - 1 == indexDataBranch && coderDefItem.j - 1 == indexCheckBranch) return &coderDefItem;
		}
		return null;
	};

	int CoderDefinition::GetDmin() {
		int dmin = INT_MAX;
		for (int i = 0; i < itemsCoderDefinition.size(); i++) {
			CoderDefinitionItem *item = &itemsCoderDefinition.at(i);
			std::vector<CoderDefinitionItem *> vecTargetItems;
			int indexData = item->i - 1;
			GetItemsForDataBranchIndex(indexData, vecTargetItems);
			int currDMin = 1;
			for (int j = 0; j < vecTargetItems.size(); j++) {
				CoderDefinitionItem *eachItem = vecTargetItems[j];
				currDMin += eachItem->powersPolynom.size();
			}
			if (currDMin < dmin) dmin = currDMin;
		}
		return dmin;
	};
}