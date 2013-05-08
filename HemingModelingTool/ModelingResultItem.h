#pragma once

class ModelingResultItem : public BaseClass {
private:
	byte *_srcBlock;
	byte *_encodedBlock;
	byte *_receivedBlock;
	byte *_decodedBlock;
	int _originalDataLen;
public:
	ModelingResultItem(int orignalDataLen);

	void ModelingResultItem::SetSourceBlock(byte *src);		
	void ModelingResultItem::SetEncodedBlock(byte *encoded);		
	void ModelingResultItem::SetReceivedBlock(byte *data);		
	void ModelingResultItem::SetDecodedBlock(byte *data);
	bool ModelingResultItem::IsResultEqualsOriginal();
};


