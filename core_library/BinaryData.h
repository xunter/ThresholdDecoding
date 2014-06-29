#pragma once

#include "core_library.h"
#include "baseclass.h"

namespace ThresholdDecoding {

	class BinaryData :
		public BaseClass
	{
	public:
		BinaryData();
		BinaryData(byte *binaryData, int length, int bitsCount);
		BinaryData(byte *binaryData, int length);
		
		virtual ~BinaryData(void);
				
		byte *GetData() const;
		void SetData(byte *binaryData, int length, int bitsCount);
		int GetDataLength();
		int GetBitsCount();

		void ShowBitsOnOutput(std::ostream &os, const char *labelText);
	private:
		byte *_binaryData;
		int _length;
		int _bitsCount;
	};
}
