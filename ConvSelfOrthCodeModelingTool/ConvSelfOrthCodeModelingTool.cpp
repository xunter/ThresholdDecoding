// HemingModelingTool.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "Coder.h"
#include "ConvSelfOrthCoder.h"

#include "DataBlockGenerator.h"
#include "RandomDataBlockGenerator.h"

#include "ByteUtil.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"
#include "ModelingResultItemStorage.h"
#include "ModelingResultConsoleItemStorage.h"
#include "GeneratingPolynom.h"
#include "BinaryMatrix.h"

void ConsoleUserPrompt() {
	cout << "Press any key to continue..." << endl;
	cin.get();
};

int main(int argc, char *argv[])
{	
	int stepCount = 1000; //Количество инетараций
	string polynom;
	float p0 = 0.01; //Вероятность искажения бита при передаче

	/*char *testArgs[6];
	testArgs[0] = "-v";
	testArgs[1] = "10";
	testArgs[2] = "-m";
	testArgs[3] = "3";
	testArgs[4] = "-p0";
	testArgs[5] = "0.01";*/

	int argsCount = argc;
		
	for (int i = 0; i < argsCount - 1; i++) {
		char *currChar = argv[i];
		char *nextChar = argv[i + 1];

		if (strcmp("-v", currChar) == 0) {
			if (i < argsCount - 1) {
				stepCount = stoi(nextChar);
			}
		}
		if (strcmp("--Polynom", currChar) == 0) {
			if (i < argsCount - 1) {
				polynom = nextChar;
			}
		}
		if (strcmp("-p0", currChar) == 0) {
			if (i < argsCount - 1) {
				p0 = stof(nextChar);
			}
		}
	}

	if (stepCount <= 0) {
		cout << "V must be greater than 0!";
		exit(1);
	}
		
	if (p0 <= 0 || p0 > 1) {
		cout << "P0 must be greater than 0 and lower than 1!";
		exit(1);
	}

	//Init random
	srand((unsigned)time(0));

	//vol
	GeneratingPolynom generatingPolynom(polynom);
	generatingPolynom.Init();
	BinaryMatrix polynomFactors = *generatingPolynom.GetPolynomFactors();
	int polynomPower = generatingPolynom.GetPolynomPower();
	int encodedBlockLen = polynomPower * 2;
	int dataBlockLen = polynomPower;
		
	cout << "Count: " << stepCount << endl;
	cout << "Data bits: " << dataBlockLen << endl;
	cout << "Encoded block bits: " << encodedBlockLen << endl;
	cout << "Noise probability: " << p0 * 100 << " %" << endl;

	byte testData = 96;
	
	ConvSelfOrthCoder *coder = new ConvSelfOrthCoder(&polynomFactors, polynomPower);
	coder->Init();

	
	//Coder *coder = new FakeCoder(dataBlockLen, m);
	DataBlockGenerator *generator = new RandomDataBlockGenerator(dataBlockLen);	
	ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, dataBlockLen);
	ModelingResultItemStorage *itemStorage = new ModelingResultConsoleItemStorage();

	float pResult = 0;
	int failsCounter = 0;
	int bitErrorCounter = 0;
	for (int i = 0; i < stepCount; i++) {
		ModelingResultItem *item = modelingEngine->Simulate();
		itemStorage->Store(item);
		if (!item->IsResultEqualsOriginal()) failsCounter++;
		bitErrorCounter += item->GetBitDiffCount();
		BaseClass::Clean(item);
	}
	
	itemStorage->Complete();

	float pNoise = (float)modelingEngine->GetNoiseCount() / (float)stepCount;
	float pBitResult = (float)bitErrorCounter / (float)(stepCount * dataBlockLen);
	pResult = (float)failsCounter / (float)stepCount;
		
	cout << "Result noise probability: " << pNoise * 100 << " %" << endl;
	cout << "Result bit probability: " << pBitResult * 100 << " %" << endl;
	cout << "Result probability: " << pResult * 100 << " %" << endl;

	BaseClass::Clean(itemStorage);
	BaseClass::Clean(modelingEngine);
	BaseClass::Clean(generator);

	ConsoleUserPrompt();
	
	return 0;
};

