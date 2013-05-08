#pragma once

#include "ModelingResultItem.h"

class ModelingResultItemStorage : public BaseClass {
public:
	virtual void Store(ModelingResultItem *item) = null;
	virtual void Complete() = null;
};

