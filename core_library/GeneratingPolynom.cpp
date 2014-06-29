#pragma once

#include "GeneratingPolynom.h"
#include "StringUtils.h"


namespace ThresholdDecoding {

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
	vector<string> *parts = StringUtils::Split(*_polynomStr, '+');
	int power = 0;
	int maxPower = 0;
	for (auto part = parts->begin(); part != parts->end(); ++part)
	{
		string partStr = *part;
		int eachPower = 0;
		if (partStr == "1") {
			eachPower = 0;
			// eachPower is already setted to 1;	
		} else if (partStr == "x") {
			eachPower = 1;
		} else {
			vector<string> *xAndPower = StringUtils::Split(partStr, '^');
			string powerStr = xAndPower->at(1);
			eachPower = atoi(powerStr.c_str());		
			delete xAndPower;
		}
		_vecPowers.push_back(eachPower);
		power = eachPower;
		if (eachPower > maxPower) maxPower = eachPower;
	}

	BinaryMatrix *factors = BinaryMatrix::CreateVector(maxPower + 1);
	for (int i = 0; i < _vecPowers.size(); i++) factors->SetItem(0, _vecPowers[i], true);
	_polynomFactors = factors;
	_polynomPower = power;
}

void GeneratingPolynom::GetFactorsVector(std::vector<int> &factors) {
	for (int i = 0; i < _polynomFactors->GetColCount(); i++) {
		bool exists = _polynomFactors->GetItem(0, i);
		if (exists) {
			factors.push_back(i);
		}
	}
};

BinaryMatrix *GeneratingPolynom::GetPolynomFactors()
{
	return _polynomFactors;
}

int GeneratingPolynom::GetPolynomPower()
{
	return _polynomPower;
}

}