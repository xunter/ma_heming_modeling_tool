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
		int mask = pow(2.0, bitPos);
		return b & mask;
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
	void LoadFromByteArray(byte *b);

	BinaryMatrix *Transpose();
	BinaryMatrix *ConcatWidth(BinaryMatrix *other);


	static BinaryMatrix *Mul(BinaryMatrix *m1, BinaryMatrix *m2);
	static BinaryMatrix *CreateIdentityMatrix(int size);
};

BinaryMatrix::BinaryMatrix(int rowSize, int colSize) {
	_row = rowSize;
	_col = colSize;

	InitMatrixArray();
};

BinaryMatrix::~BinaryMatrix() {
	for (int i = 0; i < _row; i++) {
		delete _matrixArr[i];
	}
	delete _matrixArr;
};

void BinaryMatrix::InitMatrixArray() {	
	bool **matrix = new bool*[_row];
	for (int i = 0; i < _col; i++) {
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
	BinaryMatrix *matrix = new BinaryMatrix(_col, _row);
	for (int i = 0; i < _row; i++)
		for (int j = 0; j < _col; j++)
			matrix->SetItem(j, i, GetItem(i, j));
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
				matrix->SetItem(i, j, other->GetItem(i, j));
			}
		}
	}

	return matrix;
};

BinaryMatrix *BinaryMatrix::CreateIdentityMatrix(int size) {
	BinaryMatrix *matrix = new BinaryMatrix(size, size);
	for (int i = 0; i < size; i++) matrix->SetItem(i, i, true);
	return matrix;
};

BinaryMatrix *BinaryMatrix::Mul(BinaryMatrix *m1, BinaryMatrix *m2) {
	if (m1->GetColCount() != m2->GetRowCount()) return null;
};

class HemingCoder : public Coder {
private:
	int _dataBlockLen, _entireBlockLen, _additionalBlockLen;	
	BinaryMatrix *_generatingMatrix;
	BinaryMatrix *_checkingMatrix;
	BinaryMatrix *_syndromeMatrix;
	BinaryMatrix *_identityMatrix;
	
	void InitGeneratingMatrix();
	void InitCheckingMatrix();
	void InitSyndromeMatrix();
public:
	HemingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen);
	void Init();	
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
};

HemingCoder::HemingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen) {
	_dataBlockLen = dataBlockLen;
	_entireBlockLen = entireBlockLen;
	_additionalBlockLen = additionalBlockLen;
}

void HemingCoder::Init() {
	_identityMatrix = BinaryMatrix::CreateIdentityMatrix(_dataBlockLen);
	InitGeneratingMatrix();
	InitCheckingMatrix();
	InitSyndromeMatrix();
};

byte *HemingCoder::Encode(byte* src)
{

	return null;
}

byte *HemingCoder::Decode(byte* src)
{
	return null;
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

	int stepCount = 1000; // оличество инетараций
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

	//Coder *coder = new HemingCoder();
	Coder *coder = new FakeCoder(dataBlockLen, m);
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

