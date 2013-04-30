// HemingModelingTool.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "Coder.h"
#include "HemmingCoder.h"

#include "DataBlockGenerator.h"
#include "RandomDataBlockGenerator.h"

#include "ByteUtil.h"








class ModelingResultItem : public BaseClass {
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

class ModelingEngine : public BaseClass {
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

class ModelingResultItemStorage : public BaseClass {
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

	int stepCount = 10; //Количество инетараций
	int m = 3; //Количество дополнительных разрядов
	float p0 = 0.01; //Вероятность искажения бита при передаче
	//vol

	int encodedBlockLen = pow((float)2, m) - 1;
	int dataBlockLen = encodedBlockLen - m;

	
	cout << "Count: " << stepCount << endl;
	cout << "Additional bits: " << m << endl;
	cout << "Data bits: " << dataBlockLen << endl;
	cout << "Encoded block bits: " << encodedBlockLen << endl;
	cout << "Noise probability: " << p0 * 100 << " %" << endl;

	byte testData = 96;
	

	HemmingCoder *coder = new HemmingCoder(dataBlockLen, encodedBlockLen, m);
	coder->Init();

	byte *encodedData = coder->Encode(&testData);
	byte *decodedData = coder->Decode(encodedData);


	return 0;
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

		BaseClass::Clean(item);
	}
	
	itemStorage->Complete();

	pResult = (float)failsCounter / (float)stepCount;

	cout << "Result probability: " << pResult * 100 << " %" << endl;

	BaseClass::Clean(itemStorage);
	BaseClass::Clean(modelingEngine);
	BaseClass::Clean(generator);

	ConsoleUserPrompt();

	return 0;
}

