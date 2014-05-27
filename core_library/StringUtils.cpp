#include "StdAfx.h"
#include "StringUtils.h"

namespace ThresholdDecoding {

StringUtils::StringUtils(void)
{
}


StringUtils::~StringUtils(void)
{
}

std::vector<std::string> *StringUtils::Split(const std::string &str, char delimiter)
{
	std::vector<std::string> *strings = new std::vector<std::string>();
	Split(str, delimiter, *strings);
	return strings;
}

void StringUtils::Split(const std::string &str, char delimiter, std::vector<std::string> &vecParts)
{
    std::istringstream f(str);
	std::string strBuf;
    while (std::getline(f, strBuf, delimiter)) {
		const std::string s = strBuf;
		vecParts.push_back(strBuf);
    }	
}

}