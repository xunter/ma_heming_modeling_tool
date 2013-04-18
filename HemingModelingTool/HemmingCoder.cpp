#include "StdAfx.h"
#include "HemmingCoder.h"
#include "PMatrixColGenerator.h"

HemmingCoder::HemmingCoder(int dataBlockLen, int entireBlockLen, int additionalBlockLen) {
	_dataBlockLen = dataBlockLen;
	_entireBlockLen = entireBlockLen;
	_additionalBlockLen = additionalBlockLen;
}

HemmingCoder::~HemmingCoder() {
	BaseClass::Clean(_generatingMatrix);
	BaseClass::Clean(_checkingMatrix);
	BaseClass::Clean(_pMatrix);
	BaseClass::Clean(_identityGeneratingMatrix);
	BaseClass::Clean(_identityCheckingMatrix);
};

void HemmingCoder::Init() {
	InitIdentityGeneratingMatrix();
	InitIdentityCheckingMatrix();
	InitPMatrix();
	InitGeneratingMatrix();
	InitCheckingMatrix();
};

void HemmingCoder::InitIdentityCheckingMatrix() {
	_identityCheckingMatrix = BinaryMatrix::CreateIdentityMatrix(_additionalBlockLen);
};

void HemmingCoder::InitIdentityGeneratingMatrix() {
	_identityGeneratingMatrix = BinaryMatrix::CreateIdentityMatrix(_dataBlockLen);
};

void HemmingCoder::InitCheckingMatrix() {
	_checkingMatrix = _pMatrix->ConcatWidth(_identityCheckingMatrix);
};

void HemmingCoder::InitGeneratingMatrix() {
	BinaryMatrix *pMatrixTranspose = _pMatrix->Transpose();
	_generatingMatrix = _identityGeneratingMatrix->ConcatWidth( pMatrixTranspose );
	BaseClass::Clean(pMatrixTranspose);
};

void HemmingCoder::InitPMatrix() {
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
		BaseClass::Clean(pMatrixCol);
	}
};

void HemmingCoder::FixErrorBySyndrome(BinaryMatrix *receivedVector, BinaryMatrix *syndrome) {
	byte *syndromeArr = syndrome->StoreAsByteArray();
	byte syndromeByte = syndromeArr[0];
	int fixIndex = syndromeByte;
	receivedVector->SetItem(0, fixIndex, !receivedVector->GetItem(0, fixIndex));
};

byte *HemmingCoder::Encode(byte* src)
{
	BinaryMatrix *originalDataVector = BinaryMatrix::CreateVectorFromBinaryData(src, _dataBlockLen);
	BinaryMatrix *encodedMatrix = originalDataVector->Mul(_generatingMatrix);
	byte *encodedData = encodedMatrix->StoreAsByteArray();

	BaseClass::Clean(originalDataVector);
	BaseClass::Clean(encodedMatrix);

	return encodedData;
}

byte *HemmingCoder::Decode(byte* src)
{
	BinaryMatrix *receivedVector = BinaryMatrix::CreateVectorFromBinaryData(src, _entireBlockLen);
	BinaryMatrix *syndromeVector = receivedVector->Mul(_checkingMatrix);

	bool errorOccurred = syndromeVector->IsZero();

	if (errorOccurred) {
		FixErrorBySyndrome(receivedVector, syndromeVector);
	};

	BinaryMatrix *decodedMatrix = receivedVector->Crop(0, 0, 0, _dataBlockLen);
	byte *decodedData = decodedMatrix->StoreAsByteArray();

	BaseClass::Clean(receivedVector);
	BaseClass::Clean(syndromeVector);
	BaseClass::Clean(decodedMatrix);

	return decodedData;
}