// HemingModelingTool.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <fstream>
#include "Coder.h"

#include "DataBlockGenerator.h"
#include "RandomDataBlockGenerator.h"
#include "BitDataBlockGenerator.h"

#include "ByteUtil.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"
#include "ModelingResultItemStorage.h"
#include "ModelingResultConsoleItemStorage.h"
#include "BinaryMatrix.h"
#include "ConvSelfOrthCoder.h"
#include "GeneratingPolynom.h"
#include "StringUtils.h"
#include "VectorUtils.h"

using namespace ThresholdDecoding;


//The method reads app values from the passed arguments array
void ReadArgsValuesFromArgsArray(char **argv, const int argc, std::string &polynom, int &v, float &p0) {
	bool loadPolynomFromFile = false;
	int argsCount = argc;
	for (int i = 0; i < argsCount - 1; i++) {
		char *currChar = argv[i];
		char *nextChar = argv[i + 1];

		if (strcmp("-v", currChar) == 0) {
			if (i < argsCount - 1) {
				v = atoi(nextChar);
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
				p0 = atof(nextChar);
			}
		}
	}

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
};


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

	int argsCount = argc;
	
	if (argc == 1) {
		std::string defaultArgsStr;
		char *defaultArgsFilename = "default_args.txt";
		ifstream defaultArgsIF(defaultArgsFilename);
		getline(defaultArgsIF, defaultArgsStr);
		defaultArgsIF.close();
		
		std::vector<std::string> *defaultArgsVector = StringUtils::Split(defaultArgsStr, ' ');

		char **defaultArgsArr = new char*[defaultArgsVector->size()];
		int arrCounter = 0;
		for (std::vector<string>::iterator eachStrIter = defaultArgsVector->begin(); eachStrIter != defaultArgsVector->end(); ++eachStrIter) {
			std::string eachStr = static_cast<std::string>(*eachStrIter);
			char *newStr = new char[eachStr.size()];
			strcpy(newStr, eachStr.c_str());
			defaultArgsArr[arrCounter++] = newStr;
		}
				
		ReadArgsValuesFromArgsArray(defaultArgsArr, defaultArgsVector->size(), polynom, v, p0);
		
		delete defaultArgsVector;
		delete [] defaultArgsArr;


	} else {
		ReadArgsValuesFromArgsArray(argv, argc, polynom, v, p0);
	}
	
	cout << "V = " << v << endl;
	cout << "p0 = " << p0 << endl;
	cout << "polynom = " << polynom << endl;

	n0 = 2;
	k0 = 1;
	R = (float)k0 / n0;
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
		
	GeneratingPolynom generatingPolynom(polynom);
	generatingPolynom.Init();
	BinaryMatrix *polynomFactors = generatingPolynom.GetPolynomFactors();
	int polynomPower = generatingPolynom.GetPolynomPower();
	int m = polynomPower;
	int encodedBlockLen = polynomPower * 2;
	int informationLengthBits = v;
		
	cout << "m = " << m << endl;
	cout << "V (count) = " << v << endl;
	cout << "Noise probability: " << p0 * 100 << " %" << endl;

	int dataBlockLen = 1;
	DataBlockGenerator *generator = new BitDataBlockGenerator();
	
	ConvSelfOrthCoder *coder = new ConvSelfOrthCoder(polynomFactors, polynomPower, n0, k0, true);
	coder->Init();

	ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, dataBlockLen);
	modelingEngine->SetDecoderLatency(m);
	ModelingResultItemStorage *itemStorage = new ModelingResultConsoleItemStorage();


	
	float pResult = 0;
	int failsCounter = 0;
	int bitErrorCounter = 0;
	for (int i = 0; i < v + m; i++) {
		ModelingResultItem *item = modelingEngine->Simulate();
		if (i >= m) {
			itemStorage->Store(item);
			if (!item->IsResultEqualsOriginal()) failsCounter++;
			bitErrorCounter += item->GetBitDiffCount();
		}
	}
	
	itemStorage->Complete();

	float pNoise = (float)modelingEngine->GetNoiseCount() / (float)v;
	float pBitResult = (float)bitErrorCounter / (float)(v * dataBlockLen);
	pResult = (float)failsCounter / (float)v;
		
	cout << "Result noise probability: " << pNoise * 100.0f << " %" << endl;
	cout << "Result bit probability: " << pBitResult * 100.0f << " %" << endl;
	cout << "Result probability: " << pResult * 100.0f << " %" << endl;
	
	for (std::vector<ModelingResultItem *>::iterator al = modelingEngine->GetItems()->begin(); al != modelingEngine->GetItems()->end(); ++al) {
		delete *al;
	}
	
	BaseClass::Clean(itemStorage);
	BaseClass::Clean(modelingEngine);
	BaseClass::Clean(generator);
	BaseClass::Clean(coder);
	
	ConsoleUserPrompt();
	
	return 0;
};
