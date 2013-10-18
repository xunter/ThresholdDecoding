#include "StdAfx.h"
#include "BitDataBlockGenerator.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

	BitDataBlockGenerator::BitDataBlockGenerator(void) : DataBlockGenerator(1)
	{
		_probabilityIdentityToZero = 0.5f;
	}


	BitDataBlockGenerator::~BitDataBlockGenerator(void)
	{
	}

	byte *BitDataBlockGenerator::GenerateBlock() {
		bool bitAsBool = GenerateBitAsBool();
		byte b = 0x00;
		if (bitAsBool) {
			ByteUtil::SetBit(b, 0);
		}
		byte *barr = new byte[1];
		barr[0] = b;
		return barr;
	}

	bool BitDataBlockGenerator::GenerateBitAsBool() {
		byte rndByte = ByteUtil::GenerateByte();
		bool bitAsBool = rndByte > (256 * (1.0f - _probabilityIdentityToZero));
		return bitAsBool;
	}



}