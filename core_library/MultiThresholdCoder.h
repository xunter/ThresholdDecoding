#pragma once
#include "ConvSelfOrthCoder.h"

namespace ThresholdDecoding {
	class MultiThresholdCoder :
		public ConvSelfOrthCoder
	{
	public:
		MultiThresholdCoder(int countDecoderSections, CoderDefinition *coderDefinition, int polynomPower, const int &k0, const int &n0, bool createCoderForDecoder);
		virtual ~MultiThresholdCoder(void);
		
		void Init();
		void DisplayDebugInfo(const char *label);
	protected:
		MultiThresholdCoder(void);
		void InitDecoderSectionsCoders();
		void InitDiffRegistries();
		int GetSizeDiffRegistry();
		
		void DecodeCore(bool *receivedBits, std::vector<bool> &decodedBits);

		bool IsHubDecoder();

		
		void SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, const bool &nextVal);

		void DisplayDebugInfoExternalCoder(const char *label);

		void SetFirstSection(bool val);
		bool GetLastDiffVal();
		void ShiftDiffRegistryRight(BinaryMatrix *diffRegistry);
		bool CheckThresholdConditionSyndrome(int indexOutput, std::vector<bool> &syndromes, std::vector<bool> &additionalParts);
		bool CheckThresholdCondition(int indexData, std::vector<CoderDefinitionItem *> *vecCheckBranchItems = NULL);
	private:	
		bool *_arrDiffInitVals;
		bool *_arrSyndromeInitVals;
		std::vector<BinaryMatrix *> *_diffRegistries;
		bool *_arrThresholdConditions;
		bool _firstSection;
		std::vector<MultiThresholdCoder *> *_decoderSectionsCoders;
		int _countDecoderSections;
	};
}
