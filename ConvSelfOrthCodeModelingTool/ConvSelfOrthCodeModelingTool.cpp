// HemingModelingTool.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <fstream>
#include "Coder.h"

#include "DataBlockGenerator.h"
#include "RandomDataBlockGenerator.h"

#include "ByteUtil.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"
#include "ModelingResultItemStorage.h"
#include "ModelingResultConsoleItemStorage.h"
#include "BinaryMatrix.h"
#include "ConvSelfOrthCoder.h"
#include "GeneratingPolynom.h"

void ConsoleUserPrompt() {
	cout << "Press any key to continue..." << endl;
	cin.get();
};

int main(int argc, char *argv[])
{	
	int v = 1000; //Количество инетараций
	string polynom;
	bool loadPolynomFromFile = false;
	float p0 = 0.01; //Вероятность искажения бита при передаче
	int n0, k0;
	float R;
	/*char *testArgs[6];
	testArgs[0] = "-v";
	testArgs[1] = "10";
	testArgs[2] = "-m";
	testArgs[3] = "3";
	testArgs[4] = "-p0";
	testArgs[5] = "0.01";*/

	int argsCount = argc;
#if DEBUG
	v = 1000;
	polynom = "1+x+x^4+x^6";
	p0 = 0.0001;
#else
	for (int i = 0; i < argsCount - 1; i++) {
		char *currChar = argv[i];
		char *nextChar = argv[i + 1];

		if (strcmp("-v", currChar) == 0) {
			if (i < argsCount - 1) {
				v = stoi(nextChar);
			}
		}
		if (strcmp("--Polynom", currChar) == 0) {
			if (i < argsCount - 1) {
				polynom = nextChar;
			}
		}
		if (strcmp("--PolynomFilename", currChar) == 0) {
			if (i < argsCount - 1) {
				polynom = nextChar;
				loadPolynomFromFile = true;
			}
		}
		if (strcmp("-p0", currChar) == 0) {
			if (i < argsCount - 1) {
				p0 = stof(nextChar);
			}
		}
	}

#endif
	
	n0 = 2;
	k0 = 1;
	R = k0 / n0;
	cout << "n0 = " << n0 << endl;
	cout << "k0 = " << k0 << endl;
	cout << "R = " << R << endl;

	if (v <= 0) {
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
	if (loadPolynomFromFile) {
		cout << "A polynom will be read from the \"" << polynom << "\" file." << endl;
		ifstream inputStream (polynom);
		if (inputStream.is_open()) {
			getline(inputStream, polynom);
			inputStream.close();
		} else {
			cout << "Failed to read a polynom from the \"" << polynom << "\" file!" << endl;
		}
	}
	GeneratingPolynom generatingPolynom(polynom);
	generatingPolynom.Init();
	BinaryMatrix polynomFactors = *generatingPolynom.GetPolynomFactors();
	int polynomPower = generatingPolynom.GetPolynomPower();
	int encodedBlockLen = polynomPower * 2;
	int informationLengthBits = v;
		
	cout << "V (count) = " << v << endl;
	cout << "Noise probability: " << p0 * 100 << " %" << endl;

	int dataBlockLen = 1;
	DataBlockGenerator *generator = new RandomDataBlockGenerator(dataBlockLen);
	
	ConvSelfOrthCoder *coder = new ConvSelfOrthCoder(&polynomFactors, polynomPower, n0, k0, true);
	coder->Init();

	//Coder *coder = new FakeCoder(dataBlockLen, m);
	ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, dataBlockLen);
	ModelingResultItemStorage *itemStorage = new ModelingResultConsoleItemStorage();

	float pResult = 0;
	int failsCounter = 0;
	int bitErrorCounter = 0;
	for (int i = 0; i < v; i++) {
		ModelingResultItem *item = modelingEngine->Simulate();
		itemStorage->Store(item);
		if (!item->IsResultEqualsOriginal()) failsCounter++;
		bitErrorCounter += item->GetBitDiffCount();
		BaseClass::Clean(item);
	}
	
	itemStorage->Complete();

	float pNoise = (float)modelingEngine->GetNoiseCount() / (float)v;
	float pBitResult = (float)bitErrorCounter / (float)(v * dataBlockLen);
	pResult = (float)failsCounter / (float)v;
		
	cout << "Result noise probability: " << pNoise * 100 << " %" << endl;
	cout << "Result bit probability: " << pBitResult * 100 << " %" << endl;
	cout << "Result probability: " << pResult * 100 << " %" << endl;

	BaseClass::Clean(itemStorage);
	BaseClass::Clean(modelingEngine);
	BaseClass::Clean(generator);

	ConsoleUserPrompt();
	
	return 0;
};

