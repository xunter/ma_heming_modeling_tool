// HemingModelingTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <ctime>

using namespace std;

typedef unsigned char byte;
#define null 0
#define BYTE_BIT_LEN 8

class Object {
public:
	Object();
	virtual ~Object();
	static void Clean(Object *object);
};

Object::Object() {};
Object::~Object() {};
void Object::Clean(Object *obj) {
	if (obj != null) delete obj;	
}


static class ByteUtil {
public:

	// —читать число байтов дл€ размещени€ заданного количество битов
	static int GetByteLenForDataLen(int dataLen) {
		return ceil((float)dataLen / BYTE_BIT_LEN);	
	};

	// √енерирует случайный байт
	static byte GenerateByte() {
		return (byte) (rand() % 26 + 'a');
	};

	// ”паковывает блок битов в байт начина€ со старших разр€дов
	static byte FillDataInByte(byte data, int dataLen) {
		return data & ((byte)(0xff << (BYTE_BIT_LEN - dataLen)));
	};

	// –аспечатывает содержимое массива байт с заданным размером на консоле в hex
	static void ShowDataBlockOnConsole(byte *dataBlock, int dataBlockLen) {
		cout <<  hex << setfill('0');
		int bytesCount = GetByteLenForDataLen(dataBlockLen);
		for (int i = 0; i < bytesCount; i++) {
			cout << setw(2) << int(*(dataBlock + i)) << ' ';
		}
		cout << endl;
	};

	static bool IsDataEqual(byte *dataFirst, byte *dataSecond, int dataLen) {
		for (int i = 0; i < dataLen; i++) {
			if (dataFirst[i] != dataSecond[i]) return false;
		}
		return true;
	};

	static byte *CopyData(byte *data, int len) {
		byte *copyArr = new byte[len];
		for (int i = 0; i < len; i++) copyArr[i] = data[i];
		return copyArr;
	};

	static byte GetOnlyBitByte(byte &b, int bitPos) {		
		byte mask = (byte)pow(2.0, bitPos);
		return b & mask;
	};

	static byte InvertByte(byte b) {
		for (int i = 0; i < BYTE_BIT_LEN; i++) {
			byte bitByte = GetOnlyBitByte(b, i);
			if (!IsBitSettedInByte(b, i)) {
				SetBit(b, i);
			} else {
				UnsetBit(b, i);
			}
		}
	};

	static void SetBit(byte &b, int bitPos) {
		byte ffByte = 0xff;
		b |= GetOnlyBitByte(ffByte, bitPos);
	};

	static void UnsetBit(byte &b, int bitPos) {
		byte ffByte = 0xff;
		b &= InvertByte(GetOnlyBitByte(ffByte, bitPos));
	};

	static bool IsBitSettedInByte(byte &b, int bitPos) {
		byte bitByte = GetOnlyBitByte(b, bitPos);
		return bitByte != 0x00;
	};
};

class Coder : public Object {
public:
	virtual byte *Encode(byte *src) = null;
	virtual byte *Decode(byte *src) = null;
	//virtual int ComputeEncodedBytesCount(byte *src) = null;
};

class FakeCoder : public Coder {
private:
	int _originalDataLen;
	int _additionalBitsLen;
public:
	FakeCoder(int originalDataLen, int additionalBitsLen);
	byte *Encode(byte *src);
	byte *Decode(byte *src);
};

FakeCoder::FakeCoder(int originalDataLen, int additionalBitsLen) {
	_originalDataLen = originalDataLen;
	_additionalBitsLen = additionalBitsLen;
};

byte *FakeCoder::Encode(byte *data) {
	int additionalBytesLen = ByteUtil::GetByteLenForDataLen(_additionalBitsLen);
	int encodedArrLen = _originalDataLen + additionalBytesLen;

	byte *encodedData = new byte[encodedArrLen];
	for (int i = 0; i < encodedArrLen; i++) {
		if (i + 1 < _originalDataLen) {
			encodedData[i] = data[i];
		} else {
			encodedData[i] = ByteUtil::GenerateByte();
		}
	}
	return encodedData;
};

byte *FakeCoder::Decode(byte *data) {
	int additionalBytesLen = ByteUtil::GetByteLenForDataLen(_additionalBitsLen);
	int decodedArrLen = _originalDataLen - additionalBytesLen;

	byte *decodedData = new byte[decodedArrLen];
	for (int i = 0; i < decodedArrLen; i++) {
		decodedData[i] = data[i];
	}
	return decodedData;	
};

class BinaryMatrix : public Object {
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

	byte *StoreAsByteArray();

	BinaryMatrix *Transpose();
	BinaryMatrix *ConcatWidth(BinaryMatrix *other);
	BinaryMatrix *ConcatHeight(BinaryMatrix *other);
	BinaryMatrix *Mul(BinaryMatrix *other);
	BinaryMatrix *Crop(int rowStart, int rowEnd, int colStart, int colEnd);
	int GetBitsLength();
	bool IsVector();
	bool IsZero();
	
	static BinaryMatrix *CreateIdentityMatrix(int size);
	static BinaryMatrix *CreateVector(int size);
	static BinaryMatrix *CreateVectorFromBinaryData(byte *data, int bitLen);

	static BinaryMatrix *LoadFromByteArray(byte *data, int row, int col);
};

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


class PMatrixColGenerator : public Object {
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

class HemingCoder : public Coder {
private:
	int _dataBlockLen, _entireBlockLen, _additionalBlockLen;	
	BinaryMatrix *_generatingMatrix;
	BinaryMatrix *_checkingMatrix;
	BinaryMatrix *_identityGeneratingMatrix;
	BinaryMatrix *_identityCheckingMatrix;
	BinaryMatrix *_pMatrix;
		
	void InitIdentityGeneratingMatrix();
	void InitIdentityCheckingMatrix();
	void InitPMatrix();
	void InitGeneratingMatrix();
	void InitCheckingMatrix();

	void FixErrorBySyndrome(BinaryMatrix *receivedVector, BinaryMatrix *syndrome);
public:
	HemingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen);
	~HemingCoder();
	void Init();	
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
};

HemingCoder::HemingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen) {
	_dataBlockLen = dataBlockLen;
	_entireBlockLen = entireBlockLen;
	_additionalBlockLen = additionalBlockLen;
}

HemingCoder::~HemingCoder() {
	Object::Clean(_generatingMatrix);
	Object::Clean(_checkingMatrix);
	Object::Clean(_pMatrix);
	Object::Clean(_identityGeneratingMatrix);
	Object::Clean(_identityCheckingMatrix);
};

void HemingCoder::Init() {
	InitIdentityGeneratingMatrix();
	InitIdentityCheckingMatrix();
	InitPMatrix();
	InitGeneratingMatrix();
	InitCheckingMatrix();
};

void HemingCoder::InitIdentityCheckingMatrix() {
	_identityCheckingMatrix = BinaryMatrix::CreateIdentityMatrix(_additionalBlockLen);
};

void HemingCoder::InitIdentityGeneratingMatrix() {
	_identityGeneratingMatrix = BinaryMatrix::CreateIdentityMatrix(_dataBlockLen);
};

void HemingCoder::InitCheckingMatrix() {
	_checkingMatrix = _pMatrix->ConcatWidth(_identityCheckingMatrix);
};

void HemingCoder::InitGeneratingMatrix() {
	BinaryMatrix *pMatrixTranspose = _pMatrix->Transpose();
	_generatingMatrix = _identityGeneratingMatrix->ConcatWidth( pMatrixTranspose );
	Object::Clean(pMatrixTranspose);
};

void HemingCoder::InitPMatrix() {
	PMatrixColGenerator pMatrixColGen(_dataBlockLen);
	int pRowCount = _additionalBlockLen;
	int pColCount = _dataBlockLen;
	_pMatrix = new BinaryMatrix(pRowCount, pColCount);
	
	/*
	int maxNum = pow(2.0, _additionalBlockLen + 1);
	byte *numArr = new byte[_dataBlockLen];
	int numArrIndex = 0;
	for (byte j = 1; j <= maxNum; j++) {
		numArr[numArrIndex] = j;
		numArrIndex++;
		if (++numArrIndex >= _dataBlockLen) break;
	}
	*/
	
	int twoPower = 0;

	for (int j = 0; j < pColCount; j++) {		
		BinaryMatrix *pMatrixCol = pMatrixColGen.GetNextCol();
		for (int i = 0; i < pRowCount; i++) {
			_pMatrix->SetItem(i, j, pMatrixCol->GetItem(i, 0));
		}
		Object::Clean(pMatrixCol);
	}
};

void HemingCoder::FixErrorBySyndrome(BinaryMatrix *receivedVector, BinaryMatrix *syndrome) {
	byte *syndromeArr = syndrome->StoreAsByteArray();
	byte syndromeByte = syndromeArr[0];
	int fixIndex = syndromeByte;
	receivedVector->SetItem(0, fixIndex, !receivedVector->GetItem(0, fixIndex));
};

byte *HemingCoder::Encode(byte* src)
{
	BinaryMatrix *originalDataVector = BinaryMatrix::CreateVectorFromBinaryData(src, _dataBlockLen);
	BinaryMatrix *encodedMatrix = originalDataVector->Mul(_generatingMatrix);
	byte *encodedData = encodedMatrix->StoreAsByteArray();

	Object::Clean(originalDataVector);
	Object::Clean(encodedMatrix);

	return encodedData;
}

byte *HemingCoder::Decode(byte* src)
{
	BinaryMatrix *receivedVector = BinaryMatrix::CreateVectorFromBinaryData(src, _entireBlockLen);
	BinaryMatrix *syndromeVector = receivedVector->Mul(_checkingMatrix);

	bool errorOccurred = syndromeVector->IsZero();

	if (errorOccurred) {
		FixErrorBySyndrome(receivedVector, syndromeVector);
	};

	BinaryMatrix *decodedMatrix = receivedVector->Crop(0, 0, 0, _dataBlockLen);
	byte *decodedData = decodedMatrix->StoreAsByteArray();

	Object::Clean(receivedVector);
	Object::Clean(syndromeVector);
	Object::Clean(decodedMatrix);

	return decodedData;
}

class DataBlockGenerator : public Object {
protected:
	DataBlockGenerator();
	int _dataBlockLen;
public:
	DataBlockGenerator(int dataBlockLen);
	virtual byte *GenerateBlock() = null;

	int GetDataBlockLength();
	int GetDataBlockBytesLength();
};

DataBlockGenerator::DataBlockGenerator() {};

DataBlockGenerator::DataBlockGenerator(int dataBlockLen) {
	_dataBlockLen = dataBlockLen;
}

int DataBlockGenerator::GetDataBlockLength() { return _dataBlockLen; }

int DataBlockGenerator::GetDataBlockBytesLength() { return ByteUtil::GetByteLenForDataLen(_dataBlockLen); }

class RandomDataBlockGenerator : public DataBlockGenerator {
public:
	RandomDataBlockGenerator(int dataBlockLen);
	byte *GenerateBlock();
};

RandomDataBlockGenerator::RandomDataBlockGenerator(int dataBlockLen) : DataBlockGenerator(dataBlockLen) {};

byte *RandomDataBlockGenerator::GenerateBlock() {
	int len = _dataBlockLen;
	int byteLen = ByteUtil::GetByteLenForDataLen(len);
	byte *arr = new byte[byteLen];
	int dataBitCounter = 0;
	for (int i = 0; i < byteLen; i++) {
		byte c = ByteUtil::GenerateByte();
		dataBitCounter += BYTE_BIT_LEN;
		if (dataBitCounter > len) {
			int diff = dataBitCounter - len;
			int dataBitsLen = BYTE_BIT_LEN - diff;
			c = ByteUtil::FillDataInByte(c, dataBitsLen);
		}
		arr[i] = c;
	}		
	return arr;
}

class ModelingResultItem : public Object {
private:
	byte *_srcBlock;
	byte *_encodedBlock;
	byte *_receivedBlock;
	byte *_decodedBlock;
	int _originalDataLen;
public:
	ModelingResultItem(int orignalDataLen);

	void SetSourceBlock(byte *src) {
		_srcBlock = src;
	}
		
	void SetEncodedBlock(byte *encoded) {
		_encodedBlock = encoded;
	}
		
	void SetReceivedBlock(byte *data) {
		_receivedBlock = data;
	}
		
	void SetDecodedBlock(byte *data) {
		_decodedBlock = data;
	}

	bool IsResultEqualsOriginal() {
		return ByteUtil::IsDataEqual(_srcBlock, _decodedBlock, _originalDataLen);
	};
};

ModelingResultItem::ModelingResultItem(int originalDataLen) {
	_originalDataLen = originalDataLen;
};

class ModelingEngine : public Object {
private:
	int _dataBlockLen;
	float _pNoise;

	Coder *_coder;
	DataBlockGenerator *_generator;
	
protected:
	byte *MakeNoise(byte *data);
public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen);
	virtual ModelingResultItem *Simulate();
};

ModelingEngine::ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen) {
	_dataBlockLen = originalDataBitsLen;
	_pNoise = pNoise;
	_coder = coder;
	_generator = generator;
}

ModelingResultItem *ModelingEngine::Simulate() {	
	_generator->GetDataBlockLength();

	int dataBlockBytesLen = _generator->GetDataBlockBytesLength();

	ModelingResultItem *item = new ModelingResultItem(dataBlockBytesLen);
	byte *originalData = _generator->GenerateBlock();
	item->SetSourceBlock(originalData);

	byte *encodedData = _coder->Encode(originalData);
	item->SetEncodedBlock(encodedData);

	byte *noisedData = MakeNoise(encodedData);
	item->SetReceivedBlock(noisedData);

	byte *decodedData = _coder->Decode(noisedData);
	item->SetDecodedBlock(decodedData);

	return item;
}

byte *ModelingEngine::MakeNoise(byte *data) {
	int dataLen = _dataBlockLen;
	int bytesLen = ByteUtil::GetByteLenForDataLen(dataLen);
	byte *noisedData = ByteUtil::CopyData(data, bytesLen);

	float pNoisePercent = _pNoise * 100;

	int bitsCounter = 0;
	for (int i = 0; i < bytesLen; i++) {
		byte tempByte = noisedData[i];
		byte originalByte = tempByte;
		for (int j = 0; j < BYTE_BIT_LEN; j++) {
			bitsCounter ++;
			if (bitsCounter <= dataLen) {
				float randPercent = ((float)rand())/((float)RAND_MAX) * 100.0f;
				float needsNoise =  pNoisePercent >= randPercent;
				if (!needsNoise) continue;
				byte zeroByte = 0x00;
				byte leftPart = (0xff << (BYTE_BIT_LEN - j + 1) ) & tempByte;
				byte rightPart = (0xff >> (j + 1) ) & tempByte;
				byte bitByte = ByteUtil::GetOnlyBitByte(tempByte, j);
				tempByte = zeroByte | leftPart | rightPart;
				if (bitByte == 0x00) {
					byte ffByte = 0xff;
					tempByte |= ByteUtil::GetOnlyBitByte(ffByte, j);
				}
			}
		}
		noisedData[i] = tempByte;
	}

	return noisedData;
};

class ModelingResultItemStorage : public Object {
public:
	virtual void Store(ModelingResultItem *item) = null;
	virtual void Complete() = null;
};

class ModelingResultConsoleItemStorage : public ModelingResultItemStorage {	
public:
	void Store(ModelingResultItem *item);
	void Complete();
};

void ModelingResultConsoleItemStorage::Complete() {
	//nothing
};

void ModelingResultConsoleItemStorage::Store(ModelingResultItem *item) {
	
};

void ConsoleUserPrompt() {
	cin.get();
};

int _tmain(int argc, _TCHAR* argv[])
{
	//Init random
	srand((unsigned)time(0));

	int stepCount = 10; // оличество инетараций
	int m = 3; // оличество дополнительных разр€дов
	float p0 = 0.01; //¬еро€тность искажени€ бита при передаче
	//vol

	int encodedBlockLen = pow((float)2, m) - 1;
	int dataBlockLen = encodedBlockLen - m;

	
	cout << "Count: " << stepCount << endl;
	cout << "Additional bits: " << m << endl;
	cout << "Data bits: " << dataBlockLen << endl;
	cout << "Encoded block bits: " << encodedBlockLen << endl;
	cout << "Noise probability: " << p0 * 100 << " %" << endl;

	HemingCoder *coder = new HemingCoder(dataBlockLen, encodedBlockLen, m);
	coder->Init();
	//Coder *coder = new FakeCoder(dataBlockLen, m);
	DataBlockGenerator *generator = new RandomDataBlockGenerator(dataBlockLen);
	byte* b = generator->GenerateBlock();
	ModelingEngine *modelingEngine = new ModelingEngine(coder, generator, p0, dataBlockLen);
	ModelingResultItemStorage *itemStorage = new ModelingResultConsoleItemStorage();
	float pResult = 0;
	int failsCounter = 0;
	for (int i = 0; i < stepCount; i++) {
		ModelingResultItem *item = modelingEngine->Simulate();
		itemStorage->Store(item);
		if (!item->IsResultEqualsOriginal()) failsCounter++;

		Object::Clean(item);
	}
	
	itemStorage->Complete();

	pResult = (float)failsCounter / (float)stepCount;

	cout << "Result probability: " << pResult * 100 << " %" << endl;

	Object::Clean(itemStorage);
	Object::Clean(modelingEngine);
	Object::Clean(generator);

	ConsoleUserPrompt();

	return 0;
}

