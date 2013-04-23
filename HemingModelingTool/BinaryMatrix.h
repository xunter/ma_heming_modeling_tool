#pragma once
#include "baseclass.h"


class BinaryMatrix : public BaseClass {
private:
	bool **_matrixArr;
	int _row;
	int _col;

	void InitMatrixArray();
public:
	BinaryMatrix(int rowSize, int colSize);
	~BinaryMatrix();

	int GetRowCount();
	int GetColCount();
	void SetItem(int row, int col, bool val);
	bool GetItem(int row, int col);
	void InvertItem(int row, int col);

	byte *StoreAsByteArray();

	BinaryMatrix *Transpose();
	BinaryMatrix *ConcatWidth(BinaryMatrix *other);
	BinaryMatrix *ConcatHeight(BinaryMatrix *other);
	BinaryMatrix *Mul(BinaryMatrix *other);
	BinaryMatrix *Crop(int rowStart, int rowEnd, int colStart, int colEnd);
	int GetBitsLength();
	bool IsVector();
	bool IsZero();

	bool IsSubMatrixEquals(int rowStart, int rowEnd, int colStart, int colEnd, BinaryMatrix *other);
	
	static BinaryMatrix *CreateIdentityMatrix(int size);
	static BinaryMatrix *CreateVector(int size);
	static BinaryMatrix *CreateVectorFromBinaryData(byte *data, int bitLen);

	static BinaryMatrix *LoadFromByteArray(byte *data, int row, int col);
};
