#pragma once

#include <vector>

class VectorUtils
{
public:
	VectorUtils(void);
	~VectorUtils(void);

	template <typename T>
	static T *ToArray(const std::vector<T> &vector);
};

