#pragma once
#include "PerlinNoise.h"
#include "WorleyNoise.h"
class PerlinWorleyNoise
	: public PerlinNoise,
	  public WorleyNoise
{
public:
	PerlinWorleyNoise();
};

