#include "StdAfx.h"
#include "BaseClass.h"


BaseClass::BaseClass() {};
BaseClass::~BaseClass() {};
void BaseClass::Clean(BaseClass *obj) {
	if (obj != null) delete obj;	
}
