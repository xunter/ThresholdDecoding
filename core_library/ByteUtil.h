#pragma once

#include "core_library.h"

namespace ThresholdDecoding {

static class ByteUtil {
public:

	// Считать число байтов для размещения заданного количество битов
	static int GetByteLenForDataLen(int dataLen);

	// Генерирует случайный байт
	static byte GenerateByte();

	// Упаковывает блок битов в байт начиная со старших разрядов
	static byte FillDataInByte(byte data, int dataLen);

	// Распечатывает содержимое массива байт с заданным размером на консоле в hex
	static void ShowDataBlockOnConsole(byte *dataBlock, int dataBlockLen);
	static bool IsDataEqual(byte *dataFirst, byte *dataSecond, int bitLength);
	static byte *CopyData(byte *data, int len);
	static byte GetOnlyBitByte(const byte &b, int bitPos);
	static byte InvertByte(byte b);
	static void SetBitValue(byte &b, int posBit, bool bitValue);
	static void SetBit(byte &b, int bitPos);
	static void UnsetBit(byte &b, int bitPos);
	static void InvertBit(byte &b, int bitPos);
	static bool IsBitSettedInByte(const byte &b, int bitPos);

	void ByteUtil::QSort(byte *arr, int min, int max);
	void ByteUtil::SwapBytes(byte *num, byte *num2);

	static int ByteUtil::ComputeBitDiff(byte *left, byte *right, int len);

	static byte *StoreBoolArrayAsBytes(bool *boolArr, int lengthBoolArr, int &lengthByteArr);
	static byte *StoreBoolVectorAsBytes(const std::vector<bool> &vecBools, int &lenByteArray);

	static byte ReverseBitsInByte(byte original);

	static bool *ConvertBitsToBoolArray(byte *bits, int count);
	static void ConvertBitsToBoolArray(byte *bits, int count, bool *existingBoolArr);

	static void SetBitForByteData(byte *byteData, int countBits, int indexBit, bool bitValue);
	static bool GetBitForByteData(byte *byteData, int countBits, int indexBit);
	static void InvertBitInByteData(byte *byteData, int countBits, int indexBit);
	static void FindBitLocationInData(byte *data, int countBits, int indexBit, byte &foundByte, int &foundIndexByte, int &foundIndexBit);
	static byte *CopyBitsData(byte *bitsData, int lenBits);
	static bool Xor(const bool &left, const bool &right);

	static bool *CopyBoolArray(bool *arr, int len);
};
}