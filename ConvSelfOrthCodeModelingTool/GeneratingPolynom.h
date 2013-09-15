#pragma once
#include "baseclass.h";
#include "BinaryMatrix.h";

class GeneratingPolynom :
	public BaseClass
{
public:
	GeneratingPolynom(string &polynomStr);
	~GeneratingPolynom(void);
	
	void Init();
	BinaryMatrix *GetPolynomFactors();
	int GetPolynomPower();
private:
	string *_polynomStr;
	BinaryMatrix *_polynomFactors;
	int _polynomPower;

	vector<string> *SplitString(const string &str, char delimiter);
};

