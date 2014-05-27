#include "MiscUtils.h"
#include "stdafx.h"

#include <iostream>
#include <fstream>

#define _USE_MATH_DEFINES

#include <math.h>
#include <string.h>


MiscUtils::MiscUtils(void)
{
}


MiscUtils::~MiscUtils(void)
{
}

double LoadCachedSNRdB(double snrdB, double r) {
	double p0 = -1;
	std::stringstream ss;
	ss << "p0_cached_SNRdB" << snrdB << "_R" << r << ".dat";
	std::string filename = ss.str();
	
	std::ifstream ifs(filename);
	if (ifs.good()) {
		ifs >> p0;
	}
	ifs.close();
	return p0;
};

void SaveCachedSNRdB(double snrdB, double r, double p0) {
	std::stringstream ss;
	ss << "p0_cached_SNRdB" << snrdB << "_R" << r << ".dat";
	std::string filename = ss.str();

	std::ofstream ofs(filename);
	ofs << p0;
	ofs.flush();
	ofs.close();
};

double MiscUtils::ConvertSNRdBToProbability(double snrdB, double r) {
	double p0;
	p0 = LoadCachedSNRdB(snrdB, r);
	if (p0 == -1) {
		double snr = ConvertSNRdBToSNR(snrdB);
		double qArg = sqrt(2.0 * r * snr);
		p0 = ComputeQFunc(qArg);
		SaveCachedSNRdB(snrdB, r, p0);
	}
	return p0;
	
};

double MiscUtils::ConvertSNRdBToSNR(double snrDb) {
	return pow(10.0, snrDb / 10.0);
}

double QFuncIntegrand(double t) {
	return exp( - (t * t / 2.0));
};

double ComputeQFuncIntegralFrom0ToX(double x) {
	int n = 1000;
	double a = 0;
	double b = x;
	double x0 = a;
	double xn = b;
	double h = (b - a) / (double)n;
	double xi = x0;

	double firstAdd = (QFuncIntegrand(x0) + QFuncIntegrand(xn)) / 2.0;
	double sum = firstAdd * h;

	for (int i = 0; i < n - 1; i++) {
		xi += h;
		double eachIntegrandVal = QFuncIntegrand(xi);
		double eachAdd = eachIntegrandVal * h;
		sum += eachAdd;
	}

	return sum;
}

double ComputeQFuncIntegral(double x) {
	double eps = 1E-9;
	double a = x;
	double x0 = a;
	
	double xmax = x0;
	while (true)
	{
		double eachIntegrandVal = QFuncIntegrand(xmax);
		if (eachIntegrandVal < eps)
		{
			break;
		}
		xmax = xmax * 2;
	}

	while (true)
	{
		double eachIntegrandVal = QFuncIntegrand(xmax);
		if (eachIntegrandVal > eps)
		{
			break;
		}
		xmax = xmax - x0;
	}

	double b = xmax;
	double xn = b;
	
	int n = 1000;
	double h = 0;
	while (true)
	{
		h = (b - a) / n;
		if (h < eps)
		{
			break;
		}
		n = n * 10;
		if (n == 1E+9)
		{
			break;
		}
	}

	double firstAdd = (QFuncIntegrand(x0) + QFuncIntegrand(xn)) / 2.0;
	double sum = firstAdd * h;

	
	double xi = x0;

	for (int i = 0; i < n - 1; i++) {
		xi += h;
		double eachIntegrandVal = QFuncIntegrand(xi);
		double eachAdd = eachIntegrandVal * h;
		sum += eachAdd;
	}

	return sum;
}

double MiscUtils::ComputeQFunc(double arg) {
	double factor = 1.0 / (sqrt(2.0 * M_PI));
	double integralValue = ComputeQFuncIntegral(arg);
	double result = factor * integralValue;
	return result;
}

