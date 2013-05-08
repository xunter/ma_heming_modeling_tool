#include "StdAfx.h"
#include "Coder.h"
#include "DataBlockGenerator.h"
#include "ModelingResultItem.h"
#include "ModelingEngine.h"

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
