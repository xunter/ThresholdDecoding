#include "StdAfx.h"
#include "StringUtils.h"

namespace ThresholdDecoding {

StringUtils::StringUtils(void)
{
}


StringUtils::~StringUtils(void)
{
}

std::vector<std::string> *StringUtils::Split(const std::string str, const char delimiter)
{
	std::vector<std::string> *strings = new std::vector<std::string>();
    std::istringstream f(str);
    std::string s;    
    while (std::getline(f, s, delimiter)) {
        strings->push_back(s);
    }
	return strings;
}

}