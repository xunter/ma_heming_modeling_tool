#pragma once

class ModelingEngine : public BaseClass {
private:
	int _dataBlockLen;
	float _pNoise;

	Coder *_coder;
	DataBlockGenerator *_generator;
	
public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen);
	ModelingResultItem *Simulate();
	byte *MakeNoise(byte *data);
};
