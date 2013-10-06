#pragma once
#include "StdAfx.h"
#include "GeneratingPolynom.h"


GeneratingPolynom::GeneratingPolynom(string &polynomStr)
{
	_polynomStr = &polynomStr;
}


GeneratingPolynom::~GeneratingPolynom(void)
{
	BaseClass::Clean(_polynomFactors);
}

void GeneratingPolynom::Init()
{
	vector<string> *parts = SplitString(*_polynomStr, '+');
	int power = 0;
	BinaryMatrix *factors = new BinaryMatrix(1, parts->size());
	
	for (auto part = parts->begin(); part != parts->end(); ++part)
	{
		string partStr = *part;
		int eachPower = 0;
		if (partStr == "1") {
			// eachPower is already setted to 1;	
		} else if (partStr == "x") {
			eachPower = 1;
		} else {
			vector<string> *xAndPower = SplitString(partStr, '^');
			string powerStr = xAndPower->at(1);
			eachPower = atoi(powerStr.c_str());		
		}
		power = eachPower;
		factors->SetItem(0, eachPower, true);
	}
	_polynomFactors = factors;
	_polynomPower = power;
}

vector<string> *GeneratingPolynom::SplitString(const string &str, char delimiter)
{
	vector<string> *strings = new vector<string>();
    std::istringstream f(str);
    std::string s;    
    while (std::getline(f, s, delimiter)) {
        std::cout << s << std::endl;
        strings->push_back(s);
    }
	return strings;
}

BinaryMatrix *GeneratingPolynom::GetPolynomFactors()
{
	return _polynomFactors;
}

int GeneratingPolynom::GetPolynomPower()
{
	return _polynomPower;
}
