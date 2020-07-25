/*
	Description:
	Worley noise class.

	Created:
	6/27/2020
	Last Updated:
	7/25/2020

	To Do:
	>Compute Shader. (Optional)
	>Optimization.
	See http://webstaff.itn.liu.se/~stegu/GLSL-cellular/GLSL-cellular-notes.pdf for
	possible optimizations. (2x2x2 search region is not applicable to my program)
*/


#pragma once
#include "../core/math/Math.h"
#include <vector>

class WorleyNoise
{
public:
	WorleyNoise(std::size_t resolution = 128, std::size_t cellsNums = 6, int seed = 0);

	float Noise(float x, float y, float z);
	float FBMNoise(float x, float y, float z, std::size_t octaves, float lacunarity = 2.f, float gain = 0.707f);

private:
	std::size_t resolution; //the resolution of noise
	std::size_t cellsNums;	//the number of cells

	std::vector<Vector3> featurePoints;
};