/*
Программное средство для моделирования системы связи с помехами для получения результатов эффективности методов коррекции ошибок.
Реализованы декодеки для: кодов Хемминга, порогового декодирования, многопорогового декодирования.
Данная программа является частью магистерской диссертации на тему 
"ПОВЫШЕНИЕ ЭФФЕКТИВНОСТИ МНОГОПОРОГОВОГО ДЕКОДИРОВАНИЯ ЗА СЧЕТ ВЫБОРА НАИБОЛЕЕ ЭФФЕКТИВНОГО САМООРТОГОНАЛЬНОГО КОДА"
по направлению 231000 – Программная инженерия.
Диссертант: студент-магистрант группы 843М Назаров Павел Андреевич
Научный руководитель: д.т.н., доцент кафедры ВПМ Овечкин Геннадий Владимирович
2014 год
*/
#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <string>
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
#include "MultiThresholdCoder.h"

using namespace ThresholdDecoding;

void ConsoleUserPrompt() {
	system("pause");
};


void CalculateSNRdBToP0FromZTable(float snrdB, double &p0) {
	char *filename = "snrdB_p0_table.txt";
	std::ifstream ifs(filename);
	std::string str;
	while (true) {
		getline(ifs, str);
		std::vector<std::string> vecParts;
		StringUtils::Split(str, ' ', vecParts);
		std::string &snrdbStr = vecParts[0];
		int indexComma = snrdbStr.find(',');
		snrdbStr[indexComma] = '.';
		double snrdbFromFile;
		std::stringstream(snrdbStr) >> snrdbFromFile;
		if (snrdbFromFile == snrdB) {
			std::string pStr = vecParts[1];
			std::vector<std::string> vecP0Parts;
			StringUtils::Split(pStr, 'e', vecP0Parts);
			std::string mPartStr = vecP0Parts[0];
			std::string pPartStr = vecP0Parts[1];
			char sign = pPartStr[0];
			pPartStr = pPartStr.substr(3);
			mPartStr[mPartStr.find(',')] = '.';
			double m = atof(mPartStr.c_str());
			double p = atof(pPartStr.c_str());
			p0 = m;
			for (int i = 0; i < p; i++) {
				if (sign == '-') p0 /= 10;
				else p0 *= 10;
			}
			break;
		}
	}
	ifs.close();
};

//The method reads app values from the passed arguments array
void ReadArgsValuesFromArgsArray(std::vector<std::string> &vecArgs, std::string &polynom, long &v, double &p0, float &snrdB, long &m, float &R, std::string &filenameCoderDefinition, std::string &filenameResults, bool &useRawResultData, int &decoderType, int &countSections, std::string &filenameDefaultArgs, int &p0ComputingMode) {
	int argc = vecArgs.size();
	const char *DEFAULT_filenameCoderDefinition = "coder_definition.txt";
	const char *DEFAULT_filenameResults = "results.txt";
	const char *DEFAULT_filenameDefaultArgs = "default_args.txt";
	
	filenameCoderDefinition = "";
	filenameResults = "";
	filenameDefaultArgs = "";
	
	bool loadPolynomFromFile = false;
	int argsCount = argc;
	for (int i = 0; i < argsCount - 1; i++) {
		const char *currChar = vecArgs[i].c_str();
		const char *nextChar = vecArgs[i + 1].c_str();
		
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

		if (strcmp("--filenameCoderDefinition", currChar) == 0) {			
			if (i < argsCount - 1) {
				filenameCoderDefinition = nextChar;
			}
		}
		if (strcmp("--filenameResults", currChar) == 0) {			
			if (i < argsCount - 1) {
				filenameResults = nextChar;
			}
		}
		
		if (strcmp("--filenameDefaultArgs", currChar) == 0) {			
			if (i < argsCount - 1) {
				filenameDefaultArgs = nextChar;
			}
		}
		
		if (strcmp("--useRawResultData", currChar) == 0) {
			if (i < argsCount - 1) {
				useRawResultData = strcmp(nextChar, "1") == 0;
			}
		}
		
		if (strcmp("--decoderType", currChar) == 0) {
			if (i < argsCount - 1) {
				decoderType = strcmp(nextChar, "basic") == 0 ? 1 : 2;
				if (strcmp(nextChar, "none") == 0) decoderType = 0;
			}
		}
		
		if (strcmp("--countSections", currChar) == 0) {
			if (i < argsCount - 1) {
				countSections = atoi(nextChar);
			}
		}
		
		if (strcmp("--p0ComputingMode", currChar) == 0) {
			if (i < argsCount - 1) {
				p0ComputingMode = atoi(nextChar);
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
	
	if (filenameCoderDefinition == "") filenameCoderDefinition = DEFAULT_filenameCoderDefinition;
	if (filenameResults == "") filenameResults = DEFAULT_filenameResults;
	if (filenameDefaultArgs == "") filenameDefaultArgs = DEFAULT_filenameDefaultArgs;
};

bool ReadCoderDefinitionItem(ifstream &inputStream, CoderDefinitionItem &item, bool &branchIndexZeroBased) {
	if (inputStream.eof()) {
		return false;
	}
	std::string line;
	getline(inputStream, line);
	if (line.find("Общее время эксперимента:") != std::string::npos) {
		return false;
	}
	std::vector<std::string> *termsAll = StringUtils::Split(line, ' ');
	std::vector<std::string> *terms = new std::vector<std::string>();
	for (int i = 0; i < termsAll->size(); i++) {
		std::string termStr = termsAll->at(i);
		if (!termStr.empty()) {
			terms->push_back(termStr);
		}
	}
	delete termsAll;
	std::map<std::string, std::string> mapProps;
	for (int i = 0; i < terms->size(); i++) {
		std::string termString = terms->at(i);
		int indexEqual = termString.find('=');
		if (indexEqual == std::string::npos && i == 2) {
			std::stringbuf sb;
			sb.sputc('c');
			sb.sputc('=');
			sb.sputn(termString.c_str(), termString.length());
			termString = sb.str();
		}
		if (i == 3 && indexEqual == std::string::npos) {
			std::stringbuf sb;
			bool firstPart = true;
			int j = i;
			while (j < terms->size()) {
				std::string eachNextTermStr;
				if (firstPart) {
					eachNextTermStr = termString;
				} else {
					eachNextTermStr = terms->at(j);
					sb.sputc('+');
				}
				if (eachNextTermStr == "0") {
					sb.sputc('1');
				} else {
					sb.sputc('x');
					sb.sputc('^');
					sb.sputn(eachNextTermStr.c_str(), eachNextTermStr.length());
				}
				firstPart = false;
				j++;
			}
			termString = sb.str();
			mapProps["p"] = termString;
			break;
		}
		std::vector<std::string> *pairs = StringUtils::Split(termString, '=');
		std::string key = pairs->at(0);
		if (key == "v") {
			key = "j";
		}
		std::string val = pairs->at(1);
		if ((key == "i" || key == "j") && val == "0" && !branchIndexZeroBased) {
			branchIndexZeroBased = true;
		}
		if (branchIndexZeroBased && (key == "i" || key == "j")) {
			int valNum = atoi(val.c_str());
			valNum++;
			char *buffer = new char[256];
			itoa(valNum, buffer, 10);
			val = buffer;
		}
		mapProps[key] = val;
		delete pairs;
	}
	delete terms;
	item.i = atoi( mapProps.at("i").c_str() );
	item.j = atoi( mapProps.at("j").c_str() );
	item.countBranches = 0;
	if (mapProps.count("c") > 0) {
		item.countBranches = atoi( mapProps.at("c").c_str() );
	}
	if (mapProps.count("p") > 0) {
		std::string polynomStr = mapProps["p"];
		GeneratingPolynom polynom(polynomStr);
		polynom.Init();
		polynom.GetFactorsVector(item.powersPolynom);
	}
	return true;
};

void LoadCoderDefinitionItemsFromFile(std::string &filename, std::vector<CoderDefinitionItem> &items, bool useRawResultData = false) {
	std::string polynomText = "";
	bool branchIndexZeroBased = false;
	ifstream inputStream (filename.c_str());
	if (useRawResultData) {

		char lineBuf[32768];
		
		bool foundSmin = false;
		while (!foundSmin) {
			inputStream.getline(lineBuf, 32768);
			foundSmin = strstr(lineBuf, "Smin") != 0;
		}
		if (foundSmin) {
			inputStream.getline(lineBuf, 1024);
		}
	}
	while (true) {
		CoderDefinitionItem item;
		if (ReadCoderDefinitionItem(inputStream, item, branchIndexZeroBased)) {
			items.push_back(item);
		} else {
			break;
		}
	}
	inputStream.close();
};

void SaveResultsToFile(TotalSimulationResult *totalResult, double &p0, float &snrdB, std::string &filename, time_t &timeStart, time_t &timeEnd) {
	ofstream fout;
	fout.open (filename);
		
	double pNoise = totalResult->pNoise;
	double pBitResult = totalResult->pBitResult;
	double pResult = totalResult->pResult;
	double pBlock = totalResult->pBlock;	
		
	fout << "Time_start = " << std::ctime(&timeStart);
	fout << "Time_end = " << std::ctime(&timeEnd);
	fout << "Total_iterations = " << std::scientific << std::setbase(10) << totalResult->totalCountIterations << endl;	
	fout << "SNRdB = " << snrdB << endl;
	fout << "P0 = " << std::scientific << p0 << endl;
	fout << "Pb = " << std::scientific << pBitResult << endl;
	fout << "PB = " << std::scientific << pBlock << endl;
	
	fout.close();
};

char *GetArgValue(char **argv, int argc, char *nameArg) {
	for (int i = 0; i < argc; i++) {
		char *str = argv[i];
		char *nextStr = null;
		if (i < argc - 1) {
			nextStr = argv[i + 1];
		}
		if (strcmp(str, nameArg) == 0) {
			return nextStr;
		}
	}
	return NULL;
};

bool CheckArgExist(char **argv, int argc, char *nameArg) {
	char *valueActual = GetArgValue(argv, argc, nameArg);
	bool exist = valueActual != NULL;
	return exist;
};

int main(int argc, char *argv[])
{	
	//try {
		long v = 0; //Количество итераций
		long mMax = 0; //Количество ошибочных битов на выходе декодера
		long mCounter = 0; // M = 1000, Volume
		string polynom;
		bool loadPolynomFromFile = false;
		double p0 = 0.0; //Вероятность искажения бита при передаче
		int n0, k0;
		float R;
		float snrdB;
		std::string filenameCoderDefinition;
		std::string filenameResults;
		std::string filenameDefaultArgs = "default_args.txt";
		int argsCount = argc;
		bool useRawResultData = false;
		int decoderType = 1; //1 - basic, 2 - multi
		int countSections = 1;
		std::vector<std::string> vecArgs;
		int p0ComputingMode = 0;

		bool filenameDefaultArgsSpecified = false;		
		filenameDefaultArgsSpecified = CheckArgExist(argv, argc, "--filenameDefaultArgs");
		if (filenameDefaultArgsSpecified) {
			filenameDefaultArgs = GetArgValue(argv, argc, "--filenameDefaultArgs");
		}

		if (argc == 1) {
			std::string defaultArgsStr;
			const char *defaultArgsFilename = filenameDefaultArgs.c_str();
#if _DEBUG
			defaultArgsFilename = "C:\\Users\\xunter\\SkyDrive\\Documents\\Education\\ma_diser\\my_decoding_tool\\default_args.txt";
			
#endif
			ifstream defaultArgsIF(defaultArgsFilename);
			getline(defaultArgsIF, defaultArgsStr);
			defaultArgsIF.close();

		
			StringUtils::Split(defaultArgsStr, ' ', vecArgs);
				
		} else {
			for (int i = 0; i < argc; i++) {
				std::string str = argv[i];
				vecArgs.push_back(str);
			}
		}
	
		ReadArgsValuesFromArgsArray(vecArgs, polynom, v, p0, snrdB, mMax, R, filenameCoderDefinition, filenameResults, useRawResultData, decoderType, countSections, filenameDefaultArgs, p0ComputingMode);
		
		std::vector<CoderDefinitionItem> coderDefinitionItems;
		
		LoadCoderDefinitionItemsFromFile(filenameCoderDefinition, coderDefinitionItems, useRawResultData);
	
		CoderDefinition coderDefinition;
		coderDefinition.itemsCoderDefinition = coderDefinitionItems;
		k0 = coderDefinition.GetCountInputs();
		
		if (decoderType == 0) {
			n0 = k0;
		} else {
			n0 = coderDefinition.GetCountOutputs();
		}

		R = (float)k0 / n0;

		coderDefinition.k0 = k0;
		coderDefinition.n0 = n0;
		coderDefinition.R = R;

		if (p0 == 0) {
			//SNRdB = 5 p0 = 0.037678987
			//SNRdB = 6 p0 = 0.023007140
			if (p0ComputingMode == 0) {
				p0 = MiscUtils::ConvertSNRdBToProbability(snrdB, R);
			} else if (p0ComputingMode == 1) {
				CalculateSNRdBToP0FromZTable(snrdB, p0);
			}

			//CalculateSNRdBToP0(snrdB, p0);
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
		cout << "Noise probability: " << std::scientific << p0 << endl;
		time_t time_start;
		time(&time_start);

		DataBlockGenerator *generator = new RandomDataBlockGenerator(k0);
	
		ConvSelfOrthCoder *coder = null;
		int decoderLatency = m;
		if (decoderType == 0) {
			decoderLatency = 0;
		} else if (decoderType == 1) {		
			coder = new ConvSelfOrthCoder(&coderDefinition, m, k0, n0, true);
		} else if (decoderType == 2) {
			coder = new MultiThresholdCoder(countSections, &coderDefinition, m, k0, n0, true);
			decoderLatency = decoderLatency * countSections;
		}

		if (coder != NULL) coder->Init();

		ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, k0);
		modelingEngine->SetDecoderLatency(decoderLatency);
#if _DEBUG
		//modelingEngine->SetDebugMode(true);
#endif
		DataTransmissionChannel *channel = new BinarySymmetricChannel(p0);
		modelingEngine->SetChannel(channel);
		modelingEngine->Init();

		TotalSimulationResult * totalResult = modelingEngine->SimulateTotal(mCounter, mMax);
		ModelingResultItemStorage *itemStorage = new ModelingResultConsoleItemStorage();
		itemStorage->StoreBatch(totalResult->Items);
		itemStorage->Complete();
	
		time_t time_end;
		time(&time_end);

		double pNoise = totalResult->pNoise;
		double pBitResult = totalResult->pBitResult;
		double pResult = totalResult->pResult;
		double pBlock = totalResult->pBlock;	
			
		cout << "Time_start = " << std::ctime(&time_start);
		cout << "Time_end = " << std::ctime(&time_end);
		cout << "Total iterations = " << std::scientific << std::setbase(10) << totalResult->totalCountIterations << endl;
		cout << "SNRdB = " << snrdB << endl;
		cout << "P0 = " << std::scientific << p0 << endl;
		cout << "Pb = " << std::scientific << pBitResult << endl;
		cout << "PB = " << std::scientific << pBlock << endl;
	
		SaveResultsToFile(totalResult, p0, snrdB, filenameResults, time_start, time_end);
		
		BaseClass::Clean(itemStorage);
		delete totalResult;
		BaseClass::Clean(modelingEngine);
		delete channel;
		BaseClass::Clean(coder);
		BaseClass::Clean(generator);
	return 0;
};
