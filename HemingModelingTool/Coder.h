#pragma once
#include "baseclass.h"

class Coder : public BaseClass {
public:
	virtual byte *Encode(byte *src) = null;
	virtual byte *Decode(byte *src) = null;
};

