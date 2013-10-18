#pragma once
#include "datablockgenerator.h"

namespace ThresholdDecoding {

	class BitDataBlockGenerator :
		public DataBlockGenerator
	{
	public:
		BitDataBlockGenerator(void);
		virtual ~BitDataBlockGenerator(void);
		byte *GenerateBlock();
		bool GenerateBitAsBool();
	private:
		float _probabilityIdentityToZero;
	};

}