#pragma once
class MiscUtils
{
public:
	MiscUtils(void);
	~MiscUtils(void);

	static double ConvertSNRdBToProbability(double snrdB, double r);
	static double ConvertSNRdBToSNR(double snrDb);
private:
	static double ComputeQFunc(double arg);
};

