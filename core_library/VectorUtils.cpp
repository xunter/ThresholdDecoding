#include "StdAfx.h"
#include "VectorUtils.h"


VectorUtils::VectorUtils(void)
{
}


VectorUtils::~VectorUtils(void)
{
}

template <typename T>
static T *VectorUtils::ToArray(const std::vector<T> &vector) {
	T *arr = new T[vector.size()];
	int arrCounter = 0;
	for (std::vector<string>::iterator eachStrIter = defaultArgsVector->begin(); eachStrIter != defaultArgsVector->end(); ++eachStrIter) {
		std::string eachStr = static_cast<std::string>(*eachStrIter);
		char *newStr = new char[eachStr.size()];
		strcpy(newStr, eachStr.c_str());
		arr[arrCounter++] = newStr;
	}
	return arr;
};
