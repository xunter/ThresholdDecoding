#pragma once

#include "core_library.h";
#include "baseclass.h";
#include "BinaryMatrix.h";
#include "BinaryVector.h";

namespace ThresholdDecoding {

	using namespace std;

class GeneratingPolynom :
	public BaseClass
{
public:
	GeneratingPolynom(string &polynomStr);
	~GeneratingPolynom(void);
	
	void Init();
	BinaryMatrix *GetPolynomFactors();
	void GetFactorsVector(std::vector<int> &factors);
	int GetPolynomPower();
private:
	string *_polynomStr;
	BinaryMatrix *_polynomFactors;
	std::vector<int> _vecPowers;
	int _polynomPower;

};

}