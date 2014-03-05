
#include <cppunit\TestFixture.h>
#include <cppunit\TestCase.h>
#include <cppunit\TestResult.h>
#include <cppunit\TestCaller.h>
#include <cppunit\ui\text\TestRunner.h>

#include <core_library.h>
#include <ByteUtil.h>
#include <ConvSelfOrthCoder.h>
#include <DataBlockGenerator.h>
#include <ModelingResultItem.h>
#include <ModelingEngine.h>
#include <BinaryMatrix.h>

using namespace std;
using namespace ThresholdDecoding;

class TestChannel : public DataTransmissionChannel {
public:
	void SetNoiseMask(bool *noiseMask, int len) {
		_noiseMask = noiseMask;
		_lenMask = len;
	};

	byte *HandleData(byte *data, int lenData) {
		int lenBytes = ByteUtil::GetByteLenForDataLen(lenData);
		byte *copiedData = ByteUtil::CopyBitsData(data, lenData);
		if (_noiseMask != null) {
			ApplyNoiseMask(copiedData, lenData);
		}
		return copiedData;
	};
private:
	int _lenMask;
	bool *_noiseMask;

	void ApplyNoiseMask(byte *data, int lenData) {
		for (int i = 0; i < lenData; i++) {
			bool item = _noiseMask[i];
			if (item) {
				ByteUtil::InvertBitInByteData(data, lenData, i);
			}
		}
	};
};

class TestDataBlockGenerator : public DataBlockGenerator {
public:
	int GetDataBlockBytesLength() { return 1; };
	int GetDataBlockLength() { return 1; };

	TestDataBlockGenerator(int lenBits) : DataBlockGenerator(lenBits) {
		_nextBytes = null;
		this->InitDefaultNextBytes();
	};

	void SetNextBits(bool *nextBits) {
		int lenData = 0;
		byte *bytes = null;
		int lenBits = this->GetDataBlockLength();
		bytes = ByteUtil::StoreBoolArrayAsBytes(nextBits, lenBits, lenData);
		this->SetNextBytes(bytes);
	};

	void SetNextBytes(byte *nextBytes) {
		this->_nextBytes = nextBytes;
	};

	byte *GenerateBlock() {
		return this->_nextBytes;
	};
private:
	byte *_nextBytes;

	void InitDefaultNextBytes() {
		int lenBits = this->GetDataBlockLength();
		bool *arrBits = new bool[lenBits];
		for (int i = 0; i < lenBits; i++) {
			arrBits[i] = false;
		}
		int lenBytes;
		byte *initialNextBytes = ByteUtil::StoreBoolArrayAsBytes(arrBits, lenBits, lenBytes);
		this->_nextBytes = initialNextBytes;
		delete [] arrBits;
	}

};

class ByteUtilTestFixture : public CppUnit::TestFixture {
public:

	void test_ConvertBitsToBoolArray_should_convert_all_false() {
		int countBits = 2;
		byte *src = new byte[1];
		src[0] = 0x00;
		bool *boolArr = ByteUtil::ConvertBitsToBoolArray(src, countBits);
		CPPUNIT_ASSERT( boolArr[0] == false );
		CPPUNIT_ASSERT( boolArr[1] == false );
	};
	
	void test_ConvertBitsToBoolArray_should_convert_after_store() {
		int countBits = 2;
		bool *boolArr = new bool[countBits];
		boolArr[0] == false;
		boolArr[1] == false;

		int lenBytes;
		byte *src = ByteUtil::StoreBoolArrayAsBytes(boolArr, countBits, lenBytes);
		CPPUNIT_ASSERT( src[0] == 0x00 );
		boolArr = ByteUtil::ConvertBitsToBoolArray(src, countBits);
		CPPUNIT_ASSERT( boolArr[0] == false );
		CPPUNIT_ASSERT( boolArr[1] == false );
	};

	void test_invert_bit_in_byte_data() {
		bool bitsArr[5] = { 1, 1, 0, 1, 1 };
		int lenBytes;
		ThresholdDecoding::byte *bytes = ThresholdDecoding::ByteUtil::StoreBoolArrayAsBytes(bitsArr, 5, lenBytes);
		bool thirdBoolOriginal = ThresholdDecoding::ByteUtil::GetBitForByteData(bytes, 5, 2);
		CPPUNIT_ASSERT( thirdBoolOriginal == false );
		ThresholdDecoding::ByteUtil::InvertBitInByteData(bytes, 5, 2);
		bool thirdBoolInverted = ThresholdDecoding::ByteUtil::GetBitForByteData(bytes, 5, 2);
		CPPUNIT_ASSERT( thirdBoolInverted == true );
	};

	void test_set_bit_in_byte_data() {
		int lenBytes = 3;
		ThresholdDecoding::byte *byteData = new ThresholdDecoding::byte[lenBytes];
		int lenBits = 21;
		bool arrBool[21] = { true, false, true, false, true, false, true, true, false, true, false, true, false, true, true, false, true, false, true, false, true };
		byteData = ThresholdDecoding::ByteUtil::StoreBoolArrayAsBytes(arrBool, lenBits, lenBytes);
		CPPUNIT_ASSERT( lenBytes == 3 );
		
		bool firstBitInitialValue = ThresholdDecoding::ByteUtil::GetOnlyBitByte(byteData[0], 0);
		CPPUNIT_ASSERT( firstBitInitialValue == true );
		ThresholdDecoding::ByteUtil::SetBitForByteData(byteData, lenBits, 0, 0);
		bool firstBitNewValue = ThresholdDecoding::ByteUtil::GetOnlyBitByte(byteData[0], 0);
		CPPUNIT_ASSERT( firstBitNewValue == false );
				
		bool eightthBitInitialValue = ThresholdDecoding::ByteUtil::GetOnlyBitByte(byteData[1], 0);
		CPPUNIT_ASSERT( eightthBitInitialValue == false );
		ThresholdDecoding::ByteUtil::SetBitForByteData(byteData, lenBits, 8, true);
		bool eightthBitNewValue = ThresholdDecoding::ByteUtil::GetOnlyBitByte(byteData[1], 0);
		CPPUNIT_ASSERT( eightthBitNewValue == true );
		
		bool seventeenthBitInitialValue = ThresholdDecoding::ByteUtil::GetOnlyBitByte(byteData[2], 0);
		CPPUNIT_ASSERT( seventeenthBitInitialValue == true );
		ThresholdDecoding::ByteUtil::SetBitForByteData(byteData, lenBits, 16, false);
		bool seventeenthBitNewValue = ThresholdDecoding::ByteUtil::GetOnlyBitByte(byteData[2], 0);
		CPPUNIT_ASSERT( seventeenthBitNewValue == false );
	};

	void test_is_data_equal() {
		bool arrLeft[4] = { true, true, false, true };
		bool arrRight[4] = { true, true, false, true };
		int lenBytes;
		ThresholdDecoding::byte *bytesLeft = ThresholdDecoding::ByteUtil::StoreBoolArrayAsBytes(arrLeft, 4, lenBytes);
		ThresholdDecoding::byte *bytesRight = ThresholdDecoding::ByteUtil::StoreBoolArrayAsBytes(arrRight, 4, lenBytes);
		bool areSameBitArraysEqual = ThresholdDecoding::ByteUtil::IsDataEqual(bytesLeft, bytesRight, 4);
		CPPUNIT_ASSERT( areSameBitArraysEqual );
	};

	void test_byteutil_compute_bit_diff_should_equal() {
		int lenBits = 4;
		ThresholdDecoding::byte first = 0x7A;
		ThresholdDecoding::byte second = 0x7A;

		ThresholdDecoding::byte *firstArr = new ThresholdDecoding::byte[1];
		firstArr[0] = first;
		
		ThresholdDecoding::byte *secondArr = new ThresholdDecoding::byte[1];
		secondArr[0] = second;

		int countDiffs = ThresholdDecoding::ByteUtil::ComputeBitDiff(firstArr, secondArr, lenBits);
		CPPUNIT_ASSERT( countDiffs == 0 );
	};
	
	void test_byteutil_compute_bit_diff_should_not_equal() {
		int lenBits = 4;
		ThresholdDecoding::byte first = 0x7A;
		ThresholdDecoding::byte second = 0x6A;

		ThresholdDecoding::byte *firstArr = new ThresholdDecoding::byte[1];
		firstArr[0] = first;
		
		ThresholdDecoding::byte *secondArr = new ThresholdDecoding::byte[1];
		secondArr[0] = second;

		int countDiffs = ThresholdDecoding::ByteUtil::ComputeBitDiff(firstArr, secondArr, lenBits);
		CPPUNIT_ASSERT( countDiffs > 0 );
	};

	void test_convert_bits_in_byte_array_to_bool_array() {
		ThresholdDecoding::byte *bitsByteArr = new ThresholdDecoding::byte[1];
		bitsByteArr[0] = 0xD7;
		int lenBits = 4;
		bool *boolArr = ThresholdDecoding::ByteUtil::ConvertBitsToBoolArray(bitsByteArr, lenBits);
		CPPUNIT_ASSERT( boolArr[0] == true );
		CPPUNIT_ASSERT( boolArr[1] == true );
		CPPUNIT_ASSERT( boolArr[2] == false );
		CPPUNIT_ASSERT( boolArr[3] == true );
	};
	
	void test_convert_bool_array_to_bits_in_byte_array() {
		int lenBoolArr = 4;
		bool *boolArr = new bool[lenBoolArr];
		boolArr[0] = true;
		boolArr[1] = true;
		boolArr[2] = false;
		boolArr[3] = true;
		int lenByteArr;
		ThresholdDecoding::byte *byteArr = ThresholdDecoding::ByteUtil::StoreBoolArrayAsBytes(boolArr, lenBoolArr, lenByteArr);
		CPPUNIT_ASSERT( lenByteArr == 1 );
		ThresholdDecoding::byte &firstByte = byteArr[0];
		CPPUNIT_ASSERT( firstByte == 0xD0 );
	};
};


	TestChannel *_channel;
	TestDataBlockGenerator *_generator;
	ThresholdDecoding::ConvSelfOrthCoder *_coder;
	ThresholdDecoding::CoderDefinition *_coderDefinition;
	ModelingEngine *_modelingEngine;
	bool *_noiseMaskNone;
	bool *_noiseMaskDataOnly;
	bool *_noiseMaskCheckOnly;
	bool *_noiseMaskAll;

	int _k0;
	int _n0;
	float _p0;

class ConvSelfOrthCoderTestFixture : public CppUnit::TestFixture {
public:
	

	void setUp() {};

	void tearDown() {};

	static void Init() {
	
		_k0 = 0;
		_n0 = 0;
		_coder = null;
		_modelingEngine = null;
		_generator = null;
		_channel = null;


		int k0 = 1;
		int n0 = 2;
		float p0 = 0.05;
		int R = 0.5;

		_k0 = k0;
		_n0 = n0;
		_p0 = p0;
		
		bool createCoderForDecoder = true;
		CoderDefinition *coderDefinition = new ThresholdDecoding::CoderDefinition();
		coderDefinition->k0 = k0;
		coderDefinition->n0 = n0;
		coderDefinition->R = 0.5;
		ThresholdDecoding::CoderDefinitionItem coderDefItem;
		coderDefItem.i = 1;
		coderDefItem.j = 1;
		coderDefItem.countBranches = 1;
		std::vector<int> polynomPowers;
		polynomPowers.push_back(0);
		polynomPowers.push_back(1);
		polynomPowers.push_back(4);
		polynomPowers.push_back(6);
		coderDefItem.powersPolynom = polynomPowers;
		coderDefinition->itemsCoderDefinition.push_back(coderDefItem);
		_coderDefinition = coderDefinition;

		int polynomPower = coderDefinition->GetMaxM();
		int m = polynomPower;

		ConvSelfOrthCoder *coder = new ThresholdDecoding::ConvSelfOrthCoder(coderDefinition, polynomPower, k0, n0, createCoderForDecoder);
		coder->Init();
		_coder = coder;

		TestChannel *channel = new TestChannel();
		_channel = channel;

		TestDataBlockGenerator *generator = new TestDataBlockGenerator(k0);
		_generator = generator;

		ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, k0);
		modelingEngine->SetDecoderLatency(m);
		modelingEngine->SetChannel(channel);
		modelingEngine->Init();
		_modelingEngine = modelingEngine;

		_noiseMaskAll = new bool[n0];
		_noiseMaskAll[0] = true;
		_noiseMaskAll[1] = true;

		_noiseMaskDataOnly = new bool[n0];
		_noiseMaskDataOnly[0] = true;
		_noiseMaskDataOnly[1] = false;

		_noiseMaskCheckOnly = new bool[n0];
		_noiseMaskCheckOnly[0] = false;
		_noiseMaskCheckOnly[1] = true;

		_noiseMaskNone = new bool[n0];
		_noiseMaskNone[0] = false;
		_noiseMaskNone[1] = false;

		_channel->SetNoiseMask(_noiseMaskNone, n0);
	};
	
	static void Dispose() {
		delete [] _noiseMaskAll;
		delete [] _noiseMaskDataOnly;
		delete [] _noiseMaskCheckOnly;
		delete [] _noiseMaskNone;
		delete _generator;
		delete _channel;
		delete _modelingEngine;
		delete _coderDefinition;
		delete _coder;
	};
	
	void assertResultData(ModelingResultItem *item, bool encoded1, bool encoded2, bool recieved1, bool recieved2, bool decoded) {
		byte *encodedData = item->GetEncodedBlock()->GetData();
		bool *encodedBitsArr = ByteUtil::ConvertBitsToBoolArray(encodedData, _n0);
		CPPUNIT_ASSERT( encodedBitsArr[0] == encoded1 );
		CPPUNIT_ASSERT( encodedBitsArr[1] == encoded2 );

		bool *recievedBits = ByteUtil::ConvertBitsToBoolArray(item->GetReceivedBlock()->GetData(), _n0);
		CPPUNIT_ASSERT( recievedBits[0] == recieved1 );
		CPPUNIT_ASSERT( recievedBits[1] == recieved2 );

		bool *decodedBits = ByteUtil::ConvertBitsToBoolArray(item->GetDecodedBlock()->GetData(), _k0);
		CPPUNIT_ASSERT( decodedBits[0] == decoded );
	};

	void test_coder_iteration_1() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 1, 1, 1, 0);
	};
	
	void test_coder_iteration_2() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 1, 0, 0);
	};
	
	void test_coder_iteration_3() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = false;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 1, 0, 1, 0);
	};
	
	void test_coder_iteration_4() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = false;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_5() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = false;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 1, 0, 1, 0);
	};
	
	void test_coder_iteration_6() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 1, 0, 0);
	};
	
	void test_coder_iteration_7() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = false;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 1, 1, 1, 1);
	};
	
	void test_coder_iteration_8() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 1, 0, 1);
	};
	
	void test_coder_iteration_9() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 1, 0, 1, 0);
	};
	
	void test_coder_iteration_10() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_11() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 1, 0, 1, 0);
	};
	
	void test_coder_iteration_12() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 1, 0, 1);
	};
	
	void test_coder_iteration_13() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 0, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_14() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		_channel->SetNoiseMask(_noiseMaskDataOnly, _n0);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		//assertResultData(item, 1, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_15() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		_channel->SetNoiseMask(_noiseMaskCheckOnly, _n0);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		//assertResultData(item, 1, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_16() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		_channel->SetNoiseMask(_noiseMaskDataOnly, _n0);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
	};
	
	void test_coder_iteration_17() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		_channel->SetNoiseMask(_noiseMaskCheckOnly, _n0);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
	};
	
	void test_coder_iteration_18() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		_channel->SetNoiseMask(_noiseMaskAll, _n0);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
	};
	
	void test_coder_iteration_19() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		_channel->SetNoiseMask(_noiseMaskNone, _n0);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
	};
	
	void test_coder_iteration_20() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_21() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 1, 1, 1);
	};
	
	void test_coder_iteration_22() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 0, 0, 0);
	};
	
	void test_coder_iteration_23() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 1, 1, 1);
	};
	
	void test_coder_iteration_24() {
		bool *bitsArr = new bool[1];
		bitsArr[0] = true;
		_generator->SetNextBits(bitsArr);
		ModelingResultItem *item = _modelingEngine->SimulateIteration();
		assertResultData(item, 1, 0, 0, 1, 1);
	};
	private:
};


class DisposeBoolArrayTestCase : public CppUnit::TestFixture {
public:
	static const int ARR_LENGTH = 10;

	void setUp() {
		arr = new bool[ARR_LENGTH];
	};

	void test_dispose() {
		CPPUNIT_ASSERT( arr != 0 );
		delete [] arr;
		CPPUNIT_ASSERT( arr == 0 );
	};

	void tearDown() {
		
	};

private:
	bool *arr;
};

class BinaryMatrixTestFixture : public CppUnit::TestFixture {
public:
	void test_shift_registry_right_once() {
		BinaryMatrix *vector = BinaryMatrix::CreateVector(7);
		vector->SetItem(0, 0, true);
		vector->ShiftRightOnce();
		CPPUNIT_ASSERT( vector->GetItem(0, 1) == true );
		CPPUNIT_ASSERT( vector->GetItem(0, 0) == false );
	};
};

class TestSuiteManager {
public:
	static CppUnit::TestSuite *suiteCommon() {
		CppUnit::TestSuite *suite = new CppUnit::TestSuite("general");
		suite->addTest(new CppUnit::TestCaller<DisposeBoolArrayTestCase>("dispose bool array test",  &DisposeBoolArrayTestCase::test_dispose ));
		return suite;
	};

	static CppUnit::TestSuite *suiteBinaryMatrix() {
		CppUnit::TestSuite *suite = new CppUnit::TestSuite("BinaryMatrix suite");
		suite->addTest(new CppUnit::TestCaller<BinaryMatrixTestFixture>("test_shift_registry_right_once",  &BinaryMatrixTestFixture::test_shift_registry_right_once ));
		return suite;		
	};

	static CppUnit::TestSuite *suiteConvSelfOrthCoder() {
		
		CppUnit::TestSuite *suite = new CppUnit::TestSuite("ConvSelfOrthCoder suite");
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_1",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_1 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_2",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_2 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_3",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_3 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_4",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_4 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_5",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_5 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_6",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_6 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_7",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_7 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_8",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_8 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_9",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_9 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_10",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_10 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_11",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_11 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_12",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_12 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_13",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_13 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_14",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_14 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_15",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_15 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_16",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_16 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_17",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_17 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_18",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_18 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_19",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_19 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_20",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_20 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_21",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_21 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_22",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_22 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_23",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_23 ));
		suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_24",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_24 ));
		//suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_8",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_8 ));
		//suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_9",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_9 ));
		//suite->addTest(new CppUnit::TestCaller<ConvSelfOrthCoderTestFixture>("test_coder_iteration_10",  &ConvSelfOrthCoderTestFixture::test_coder_iteration_10 ));
		return suite;
	};
		
	static CppUnit::TestSuite *suiteByteUtil() {
		CppUnit::TestSuite *suite = new CppUnit::TestSuite("ByteUtil suite");

		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_convert_bool_array_to_bits_in_byte_array",  &ByteUtilTestFixture::test_convert_bool_array_to_bits_in_byte_array ));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_convert_bits_in_byte_array_to_bool_array",  &ByteUtilTestFixture::test_convert_bits_in_byte_array_to_bool_array ));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_byteutil_compute_bit_diff_should_equal",  &ByteUtilTestFixture::test_byteutil_compute_bit_diff_should_equal ));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_byteutil_compute_bit_diff_should_not_equal",  &ByteUtilTestFixture::test_byteutil_compute_bit_diff_should_not_equal ));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_byteutil_compute_bit_diff_should_not_equal",  &ByteUtilTestFixture::test_is_data_equal ));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_set_bit_in_byte_data", &ByteUtilTestFixture::test_set_bit_in_byte_data));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_ConvertBitsToBoolArray_should_convert_all_false", &ByteUtilTestFixture::test_ConvertBitsToBoolArray_should_convert_all_false));
		suite->addTest(new CppUnit::TestCaller<ByteUtilTestFixture>("test_ConvertBitsToBoolArray_should_convert_after_store", &ByteUtilTestFixture::test_ConvertBitsToBoolArray_should_convert_after_store));
		
		
		return suite;
	};
};

int main()
{
	CppUnit::TextUi::TestRunner testRunner;
	testRunner.addTest( TestSuiteManager::suiteByteUtil() );
	testRunner.addTest( TestSuiteManager::suiteBinaryMatrix() );
	ConvSelfOrthCoderTestFixture::Init();
	testRunner.addTest( TestSuiteManager::suiteConvSelfOrthCoder() );
	testRunner.run();
	system("pause");
	ConvSelfOrthCoderTestFixture::Dispose();
	return 0;
};