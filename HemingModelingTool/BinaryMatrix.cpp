#include "StdAfx.h"
#include "BinaryMatrix.h"

int BinaryMatrix::GetColCount() { return _col; };
int BinaryMatrix::GetRowCount() { return _row; };
int BinaryMatrix::GetBitsLength() { return _row * _col; };

BinaryMatrix *BinaryMatrix::Crop(int rowStart, int rowEnd, int colStart, int colEnd) {
	int newRowCount = rowEnd - rowStart + 1;
	int newColCount = colEnd - colStart + 1;
	BinaryMatrix *matrix = new BinaryMatrix(newRowCount, newColCount);
	for (int i = rowStart; i <= rowEnd; i++) {
		for (int j = colStart; j <= colEnd; j++) {
			matrix->SetItem(i - rowStart, j - colStart, GetItem(i, j));
		}
	}
	return matrix;
};

bool BinaryMatrix::IsZero() {
	for (int i = 0; i < _row; i++)
		for (int j = 0; j < _col; j++)
			if (GetItem(i, j) == true) return false;
	return true;
};

bool BinaryMatrix::IsVector() {
	return _row == 1;
};

BinaryMatrix *BinaryMatrix::LoadFromByteArray(byte *data, int row, int col) {
	BinaryMatrix *matrix = new BinaryMatrix(row, col);
	int dataLen = matrix->GetBitsLength();
	int byteArrayLen = ByteUtil::GetByteLenForDataLen(dataLen);
	int arrIndex = 0;
	int bitCounter = 0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (bitCounter >= BYTE_BIT_LEN) {
				arrIndex++;
				bitCounter = 0;
			}
			byte tempByte = data[arrIndex];
			matrix->SetItem(i, j, ByteUtil::IsBitSettedInByte(tempByte, bitCounter));
		}
	}
	return matrix;
};

byte *BinaryMatrix::StoreAsByteArray() {
	int dataLen = GetBitsLength();
	int byteArrayLen = ByteUtil::GetByteLenForDataLen(dataLen);
	byte *arr = new byte[byteArrayLen];
	int arrIndex = 0;
	int byteFillCounter = 0;
	arr[0] = 0x00;
	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < _col; j++) {
			bool item = GetItem(i, j);
			if (byteFillCounter < BYTE_BIT_LEN) {
				byteFillCounter++;
			} else {
				byteFillCounter = 0;
				arrIndex++;
				arr[arrIndex] = 0x00;
			}
			byte tempByte = arr[arrIndex];
			if (item == true) {
				ByteUtil::SetBit(tempByte, byteFillCounter);
			}
		}
	}
	return arr;
};

BinaryMatrix::BinaryMatrix(int rowSize, int colSize) {
	_row = rowSize;
	_col = colSize;

	InitMatrixArray();
};

BinaryMatrix::~BinaryMatrix() {
	for (int i = 0; i < _row; i++) {
		delete [] _matrixArr[i];
	}
	delete [] _matrixArr;
};

void BinaryMatrix::InitMatrixArray() {	
	bool **matrix = new bool*[_row];
	for (int i = 0; i < _row; i++) {
		matrix[i] = new bool[_col];
	}
	_matrixArr = matrix;
};

void BinaryMatrix::SetItem(int row, int col, bool val) {
	_matrixArr[row][col] = val;
};

bool BinaryMatrix::GetItem(int row, int col) {
	return _matrixArr[row][col];
};

BinaryMatrix *BinaryMatrix::Transpose() {
	int tRowCount = _col;
	int tColCount = _row;
	BinaryMatrix *matrix = new BinaryMatrix(tRowCount, tColCount);
	for (int i = 0; i < tRowCount; i++)
		for (int j = 0; j < tColCount; j++) {
			bool item = GetItem(j, i);
			matrix->SetItem(i, j, item);
		}
	return matrix;
};

BinaryMatrix *BinaryMatrix::ConcatWidth(BinaryMatrix *other) {
	if (_row != other->GetRowCount()) return null;

	int resultColCount = _col + other->GetColCount();
	BinaryMatrix *matrix = new BinaryMatrix(_row, resultColCount);

	int edge = _col;
	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < resultColCount; j++) {
			if (j < edge) {
				matrix->SetItem(i, j, GetItem(i, j));
			} else {
				int otherCol = j - _col;
				matrix->SetItem(i, j, other->GetItem(i, otherCol));
			}
		}
	}

	return matrix;
};

BinaryMatrix *BinaryMatrix::ConcatHeight(BinaryMatrix *other) {
	if (_col != other->GetColCount()) return null;

	int resultRowCount = _row + other->GetRowCount();
	BinaryMatrix *matrix = new BinaryMatrix(resultRowCount, _col);

	int edge = _row;
	for (int i = 0; i < _col; i++) {
		for (int j = 0; j < resultRowCount; j++) {
			if (j < edge) {
				matrix->SetItem(j, i, GetItem(j, i));
			} else {
				int otherRow = j - _row;
				matrix->SetItem(j, i, other->GetItem(otherRow, i));
			}
		}
	}

	return matrix;
};

BinaryMatrix *BinaryMatrix::Mul(BinaryMatrix *other) {
	if (_col != other->GetRowCount()) return null;
	BinaryMatrix *matrix = new BinaryMatrix(_row, other->GetColCount());
	for (int i = 0; i < _row; i++) {
		for (int j = 0; j < GetColCount(); j++) {				
			bool temp;
			for (int r = 0; r < _col; r++) {
				temp |= GetItem(i, r) & other->GetItem(r, j);
			}
			matrix->SetItem(i, j, temp);
		}
	}
	return matrix;
};

BinaryMatrix *BinaryMatrix::CreateIdentityMatrix(int size) {
	BinaryMatrix *matrix = new BinaryMatrix(size, size);
	for (int i = 0; i < size; i++) matrix->SetItem(i, i, true);
	return matrix;
};

BinaryMatrix *BinaryMatrix::CreateVector(int size) {
	BinaryMatrix *matrix = new BinaryMatrix(1, size);
	return matrix;	
};

BinaryMatrix *BinaryMatrix::CreateVectorFromBinaryData(byte *data, int bitLen) {
	BinaryMatrix *matrix = CreateVector(bitLen);
	int byteLen = ByteUtil::GetByteLenForDataLen(bitLen);

	for (int i = 0; i < byteLen; i++) {
		byte b = data[i];
		for (int j = 0; j < BYTE_BIT_LEN; j++) {
			int itemIndex = i * BYTE_BIT_LEN + j;
			if (itemIndex <= bitLen) {
				byte bitByte = ByteUtil::GetOnlyBitByte(b, j);
				if (bitByte != 0) {
					matrix->SetItem(0, itemIndex, true);
				} 
			}
		}
	}
	return matrix;
};

bool BinaryMatrix::IsSubMatrixEquals(int rowStart, int rowEnd, int colStart, int colEnd, BinaryMatrix *other) {
	for (int i = rowStart; i <= rowEnd; i++)
		for (int j = colStart; j <= colEnd; j++)
			if (GetItem(i, j) != other->GetItem())
	return true;
};
