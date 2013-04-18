#pragma once
#include "datablockgenerator.h"

class RandomDataBlockGenerator : public DataBlockGenerator {
public:
	RandomDataBlockGenerator(int dataBlockLen);
	byte *GenerateBlock();
};

