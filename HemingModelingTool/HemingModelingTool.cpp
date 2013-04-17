// HemingModelingTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

typedef byte char;
#define null 0;

class Object {
public:
	Object();
	virtual ~Object();
	static void Clean(Object *object);
};

Object::Object() {};
Object::~Object() {};
void Object::Clean(Object *obj) {
	if (obj != 0) delete obj;	
}

class Coder : public Object {
public:
	virtual byte *Encode(byte *src) = null;
	virtual byte *Decode(byte *src) = null;
	virtual int ComputeEncodedBytesCount(byte *src) = null;
};

class HemingCoder : public Coder {
private:
	int _dataBlockLen, _entireBlockLen, _additionalBlockLen;
	
public:
	HemingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen);
	virtual byte *Encode(byte *src);
	virtual byte *Decode(byte *src);
};

HemingCoder::HemingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen) {
	_dataBlockLen = dataBlockLen;
	_entireBlockLen = entireBlockLen;
	_additionalBlockLen = additionalBlockLen;
}

byte *HemingCoder::Encode(byte* src)
{

}

byte *HemingCoder::Decode(byte* src)
{

}

int HemingCoder::ComputeEncodedBytesCount(byte *src)
{
	//return pow(2, 
}

class DataBlockGenerator : public Object {
protected:
	DataBlockGenerator();
	int _dataBlockLen;
public:
	DataBlockGenerator(int dataBlockLen);
	virtual byte *GenerateBlock() = null;

	int GetDataBlockLength();
};

DataBlockGenerator::DataBlockGenerator() {};

DataBlockGenerator::DataBlockGenerator(int dataBlockLen) {
	_dataBlockLen = dataBlockLen;
}

int DataBlockGenerator::GetDataBlockLength() { return _dataBlockLen; }

class RandomDataBlockGenerator : public DataBlockGenerator {
public:
	RandomDataBlockGenerator(int dataBlockLen);
	byte *GenerateBlock();
};

RandomDataBlockGenerator::RandomDataBlockGenerator(int dataBlockLen) : DataBlockGenerator(dataBlockLen) {};

byte *RandomDataBlockGenerator::GenerateBlock() {
	int len = _dataBlockLen;
	//int byteLen = ceil((float)len / 8);
	int byteLen = len;
	byte *arr = new byte[byteLen];
	for (int i = 0; i < byteLen; i++) {
		byte c = (byte) (rand() % 26 + 'a');
		arr[i] = c;
	}		
	return arr;
}

class ModelingResultItem : public Object {
private:
	byte *_srcBlock, _encodedBlock, _receivedBlock, _decodedBlock;
public:
	ModelingResultItem();

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
};

ModelingResultItem::ModelingResultItem() {

};

class ModelingEngine : public Object {
private:
	int dataBlockLen;
	float p0;


	Coder *_coder;
	DataBlockGenerator *_generator;

public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator);
	virtual ModelingResultItem *Simulate();
};

ModelingEngine::ModelingEngine(Coder *coder, DataBlockGenerator *generator) {
	_coder = coder;
	_generator = generator;
}

ModelingResultItem *ModelingEngine::Simulate() {
	ModelingResultItem *item = new ModelingResultItem();
}

class ModelingResultItemStorage : public Object {
public:
	virtual void Store(ModelingResultItem *item) = null;
	virtual void Complete() = null;
};

int _tmain(int argc, _TCHAR* argv[])
{
	int m = 3; // оличество дополнительных разр€дов
	float p0 = 0.01; //¬еро€тность искажени€ бита при передаче
	//vol

	int encodedBlockLen = pow((float)2, m) - 1;
	int dataBlockLen = encodedBlockLen - m - 1;

	Coder *coder = new HemingCoder();
	DataBlockGenerator *generator = new RandomDataBlockGenerator();
	ModelingEngine *modelingEngine = new ModelingEngine(coder, generator);
	ModelingResultItemStorage *itemStorage = null;

	int count = 1000;

	for (int i = 0; i < count; i++) {
		ModelingResultItem *item = modelingEngine->Simulate();
		itemStorage->Store(item);
		
		Object::Clean(item);
	}

	itemStorage->Complete();

	Object::Clean(itemStorage);
	Object::Clean(modelingEngine);
	Object::Clean(generator);

	return 0;
}

