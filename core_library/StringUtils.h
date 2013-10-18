#pragma once

#include "core_library.h"
#include <vector>

namespace ThresholdDecoding {

class StringUtils
{
public:
	StringUtils(void);
	~StringUtils(void);

	static std::vector<std::string> *Split(const std::string original, const char delimiter);
};


}