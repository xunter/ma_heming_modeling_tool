#pragma once

class ModelingEngine : public BaseClass {
private:
	int _dataBlockLen;
	float _pNoise;
	float _noiseCounter;

	Coder *_coder;
	DataBlockGenerator *_generator;
	
public:
	ModelingEngine(Coder *coder, DataBlockGenerator *generator, float pNoise, int originalDataBitsLen);
	ModelingResultItem *Simulate();
	byte *MakeNoise(byte *data);
	int GetNoiseCount();
};
