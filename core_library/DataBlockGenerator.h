#pragma once
#include "baseclass.h"

namespace ThresholdDecoding {

	class DataBlockGenerator : public BaseClass {
	public:
		DataBlockGenerator(int dataBlockLen);
		virtual byte *GenerateBlock() = null;

		int GetDataBlockLength();
		int GetDataBlockBytesLength();
	protected:
		DataBlockGenerator();
	private:
		int _dataBlockLen;
	};
}
