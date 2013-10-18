#pragma once

#include "Stdafx.h"
#include "BaseClass.h"

namespace ThresholdDecoding {

BaseClass::BaseClass() {};

BaseClass::~BaseClass() {};

void BaseClass::Clean(BaseClass *obj) {
	if (obj != null) delete obj;	
};
}