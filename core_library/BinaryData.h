#pragma once

#include "core_library.h"
#include "baseclass.h"

namespace ThresholdDecoding {

	class BinaryData :
		public BaseClass
	{
	public:
		BinaryData(byte *binaryData, int length, int bitsCount);
		BinaryData(byte *binaryData, int length);
		virtual ~BinaryData(void);

		byte *GetData();
		int GetDataLength();
		int GetBitsCount();
	private:
		byte *_binaryData;
		int _length;
		int _bitsCount;
		bool _deleteData;
	};
}
