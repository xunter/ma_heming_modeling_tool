#include "StdAfx.h"
#include "PMatrixColGenerator.h"
#include "ByteUtil.h"


BinaryMatrix *PMatrixColGenerator::ConvertNumToBinaryVector(int num) {
	BinaryMatrix *matrix = new BinaryMatrix(_rowSize, 1);
	for (int i = 0; i < _rowSize; i++) {
		byte b = (byte)_currNum;
		matrix->SetItem(i, 0, ByteUtil::IsBitSettedInByte(b, i));
	}
	return matrix;
};

PMatrixColGenerator::PMatrixColGenerator(int rowSize) {
	_currNum = 0;
	_currTwoPower = 0;
	_rowSize = rowSize;
};

BinaryMatrix *PMatrixColGenerator::GetNextCol() {
	ComputeNextNum();
	return ConvertNumToBinaryVector(_currNum);
};

void PMatrixColGenerator::ComputeNextNum() {
	while (true) {
		if (++_currNum != pow(2.0, _currTwoPower++)) break;
	}
};
