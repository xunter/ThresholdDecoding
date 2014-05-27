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
	protected:
		MultiThresholdCoder(void);
		void InitDecoderSectionsCoders();
		void InitDiffRegistries();
		int GetSizeDiffRegistry();
		
		bool *DecodeCore(bool *encodedBits);

		bool IsHubDecoder();

		
		void SetNextSyndromeVal(int indexBit, BinaryMatrix *syndrome, bool nextVal);

		void SetFirstSection(bool val);
		bool GetLastDiffVal();
		void ShiftDiffRegistryRight(BinaryMatrix *diffRegistry);
		bool CheckThresholdConditionSyndrome(int indexOutput, std::vector<bool> &syndromes, std::vector<bool> &additionalParts);
		bool CheckThresholdCondition(int indexOutput);
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
