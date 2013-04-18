#pragma once
#include "baseclass.h"
#include "BinaryMatrix.h"

class PMatrixColGenerator : public BaseClass {
private:
	int _rowSize;
	int _currNum;
	int _currTwoPower;

	void ComputeNextNum();
	BinaryMatrix *ConvertNumToBinaryVector(int num);
public:
	PMatrixColGenerator(int rowSize);

	BinaryMatrix *GetNextCol();
};


