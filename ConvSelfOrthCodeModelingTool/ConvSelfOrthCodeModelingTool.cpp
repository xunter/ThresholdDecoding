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
#include "CoderDefinitions.h"
#include "ConvSelfOrthCoder.h"
#include "GeneratingPolynom.h"
#include "StringUtils.h"
#include "VectorUtils.h"
#include "MiscUtils.h"

using namespace ThresholdDecoding;


//The method reads app values from the passed arguments array
void ReadArgsValuesFromArgsArray(char **argv, const int argc, std::string &polynom, int &v, float &p0, float &snrdB, int &m, float &R) {
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
		
		if (strcmp("-m", currChar) == 0) {
			if (i < argsCount - 1) {
				m = atoi(nextChar);
			}
		}
		
		if (strcmp("-R", currChar) == 0) {
			if (i < argsCount - 1) {
				R = atof(nextChar);
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
		if (strcmp("-snrdB", currChar) == 0) {
			if (i < argsCount - 1) {
				snrdB = atof(nextChar);
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

bool ReadCoderDefinitionItem(ifstream &inputStream, CoderDefinitionItem &item) {
	if (inputStream.eof()) {
		return false;
	}
	std::string line;
	getline(inputStream, line);
	std::vector<std::string> *terms = StringUtils::Split(line, ' ');
	std::map<std::string, std::string> mapProps;
	for (int i = 0; i < terms->size(); i++) {
		std::string termString = terms->at(i);
		std::vector<std::string> *pairs = StringUtils::Split(termString, '=');
		std::string key = pairs->at(0);
		std::string val = pairs->at(1);
		mapProps[key] = val;
		delete pairs;
	}
	delete terms;
	item.i = atoi( mapProps.at("i").c_str() );
	item.j = atoi( mapProps.at("j").c_str() );
	item.countBranches = atoi( mapProps.at("c").c_str() );
	GeneratingPolynom polynom(mapProps["p"]);
	polynom.Init();
	polynom.GetFactorsVector(item.powersPolynom);
	return true;
};

void LoadCoderDefinitionItemsFromFile(std::string filename, std::vector<CoderDefinitionItem> &items) {
	std::string polynomText = "";
	ifstream inputStream (filename.c_str());
	getline(inputStream, polynomText);
	while (true) {
		CoderDefinitionItem item;
		if (ReadCoderDefinitionItem(inputStream, item)) {
			items.push_back(item);
		} else {
			break;
		}
	}
	inputStream.close();
};



void ConsoleUserPrompt() {
	system("pause");
};

int main(int argc, char *argv[])
{	
	int v = 0; // оличество инетараций
	int mMax = 0; // оличество ошибочных битов на выходе декодера
	int mCounter = 0; // M = 1000, Volume
	string polynom;
	bool loadPolynomFromFile = false;
	float p0 = 0.0; //¬еро€тность искажени€ бита при передаче
	int n0, k0;
	float R;
	float snrdB;

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
				
		ReadArgsValuesFromArgsArray(defaultArgsArr, defaultArgsVector->size(), polynom, v, p0, snrdB, mMax, R);
		
		delete defaultArgsVector;
		delete [] defaultArgsArr;


	} else {
		ReadArgsValuesFromArgsArray(argv, argc, polynom, v, p0, snrdB, mMax, R);
	}
	
	std::vector<CoderDefinitionItem> coderDefinitionItems;
	LoadCoderDefinitionItemsFromFile("coder_definition.txt", coderDefinitionItems);

	CoderDefinition coderDefinition;
	coderDefinition.itemsCoderDefinition = coderDefinitionItems;
	k0 = coderDefinition.GetCountInputs();
	n0 = coderDefinition.GetCountOutputs();

	R = (float)k0 / n0;

	if (p0 == 0 && snrdB > 0) {
		//SNRdB = 5 p0 = 0.037678987
		if (snrdB == 5) { 
			p0 = 0.037678987;
		} else {
			p0 = MiscUtils::ConvertSNRdBToProbability(snrdB, R);
		}
	}

	cout << "M = " << mMax << endl;
	cout << "SNRdB = " << snrdB << endl;
	cout << "V = " << v << endl;
	cout << "p0 = " << p0 << endl;
	cout << "polynom = " << polynom << endl;

	cout << "n0 = " << n0 << endl;
	cout << "k0 = " << k0 << endl;
	cout << "R = " << R << endl;
	
	if (p0 <= 0 || p0 > 1) {
		cout << "P0 must be greater than 0 and lower than 1!";
		exit(1);
	}

	//Init random
	srand((unsigned)time(0));
		
	int m = coderDefinition.GetMaxM();
		
	cout << "m = " << m << endl;
	cout << "V (count) = " << v << endl;
	cout << "Noise probability: " << std::scientific << p0 * 100 << " %" << endl;

	DataBlockGenerator *generator = new RandomDataBlockGenerator(k0);
	
	ConvSelfOrthCoder *coder = new ConvSelfOrthCoder(&coderDefinition, m, k0, n0, true);
	coder->Init();

	ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, k0);
	modelingEngine->SetDecoderLatency(m);
	DataTransmissionChannel *channel = new BinarySymmetricChannel(p0);
	modelingEngine->SetChannel(channel);

	TotalSimulationResult * totalResult = modelingEngine->SimulateTotal(mCounter, mMax);
	ModelingResultItemStorage *itemStorage = new ModelingResultConsoleItemStorage();
	itemStorage->StoreBatch(totalResult->Items);
	itemStorage->Complete();
	
	float pNoise = totalResult->pNoise;
	float pBitResult = totalResult->pBitResult;
	float pResult = totalResult->pResult;
	float pBlock = totalResult->pBlock;	
	
	cout << "SNRdB = " << snrdB << endl;
	cout << "p0 = " << std::scientific << p0 << endl;
	//cout << "Result noise probability: " << std::scientific << pNoise << endl;
	cout << "p_bit = " << std::scientific << pBitResult << endl;
	cout << "p_block = " << std::scientific << pBlock << endl;
	cout << "p_block_result = " << std::scientific << pResult << endl;
	//cout << "Result probability: " << std::scientific << pResult << endl;
	
	for (std::vector<ModelingResultItem *>::iterator al = modelingEngine->GetItems()->begin(); al != modelingEngine->GetItems()->end(); ++al) {
		delete *al;
	}
	
	delete totalResult;
	BaseClass::Clean(itemStorage);
	BaseClass::Clean(modelingEngine);
	BaseClass::Clean(generator);
	BaseClass::Clean(coder);
	
	ConsoleUserPrompt();
	
	return 0;
};
