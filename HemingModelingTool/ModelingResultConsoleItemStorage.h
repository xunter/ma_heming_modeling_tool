#pragma once

#include "ModelingResultItemStorage.h"

class ModelingResultConsoleItemStorage : public ModelingResultItemStorage {	
public:
	void Store(ModelingResultItem *item);
	void Complete();
};



