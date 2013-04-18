#pragma once
#include "coder.h"



class FakeCoder : public Coder {
private:
	int _originalDataLen;
	int _additionalBitsLen;
public:
	FakeCoder(int originalDataLen, int additionalBitsLen);
	byte *Encode(byte *src);
	byte *Decode(byte *src);
};