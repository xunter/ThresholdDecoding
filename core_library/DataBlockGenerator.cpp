#include "StdAfx.h"
#include "DataBlockGenerator.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {

DataBlockGenerator::DataBlockGenerator() {};

DataBlockGenerator::DataBlockGenerator(int dataBlockLen) {
	_dataBlockLen = dataBlockLen;
}

int DataBlockGenerator::GetDataBlockLength() { return _dataBlockLen; }

int DataBlockGenerator::GetDataBlockBytesLength() { return ByteUtil::GetByteLenForDataLen(_dataBlockLen); }
}