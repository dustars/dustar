/*
	Description:
	Worley noise class.
	For the purpose of optimization, this class is tailored for the cloud generation.
	Modify it if using for other purposes, such as cancelling the invertion.

	To Do:
	>Compute Shader. (Require a glsl implementation)
	>MultiThreading.
	>Optimization.
	See http://webstaff.itn.liu.se/~stegu/GLSL-cellular/GLSL-cellular-notes.pdf for
	possible optimizations.
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