#pragma once
#include "baseclass.h"

class DataBlockGenerator : public BaseClass {
protected:
	DataBlockGenerator();
	int _dataBlockLen;
public:
	DataBlockGenerator(int dataBlockLen);
	virtual byte *GenerateBlock() = null;

	int GetDataBlockLength();
	int GetDataBlockBytesLength();
};
