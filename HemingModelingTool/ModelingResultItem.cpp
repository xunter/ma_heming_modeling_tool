#include "StdAfx.h"
#include "ModelingResultItem.h"

void ModelingResultItem::SetSourceBlock(byte *src) {
	_srcBlock = src;
};
		
void ModelingResultItem::SetEncodedBlock(byte *encoded) {
	_encodedBlock = encoded;
};
		
void ModelingResultItem::SetReceivedBlock(byte *data) {
	_receivedBlock = data;
};
		
void ModelingResultItem::SetDecodedBlock(byte *data) {
	_decodedBlock = data;
};

bool ModelingResultItem::IsResultEqualsOriginal() {
	return ByteUtil::IsDataEqual(_srcBlock, _decodedBlock, _originalDataLen);
};

ModelingResultItem::ModelingResultItem(int originalDataLen) {
	_originalDataLen = originalDataLen;
};
