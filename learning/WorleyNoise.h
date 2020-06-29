/*
	Description:
	Worley noise class.
	using normalized value so that all operations are irrelevant to resolution.

	2020/6/28
	So I've decided to implement a simplified version of Worley Noise, from
	http://webstaff.itn.liu.se/~stegu/GLSL-cellular/GLSL-cellular-notes.pdf
	I've failed to implement the orginal one, but maybe try it again in the future.

	1. Periodicity? Does the current method of moduling works with FBM?

	2. Use compute shader to calculate the noise (later optimization)
	3. The distance calculation needs to be optimized (O(n) for a given location, possibly O(logN) can be achieved) (Watch https://www.youtube.com/watch?v=4QOcCGI6xOU&t=269s for a solution)
	4. Sebtisan 
*/


#pragma once
#include "../basic/math/Math.h"
#include <vector>

class WorleyNoise
{
public:
	WorleyNoise(std::size_t resolution = 128, std::size_t cellsNums = 6, int seed = 0);

	float noise(float x, float y, float z);

private:
	std::size_t resolution;
	std::size_t cellsNums;	//the resolution of cells

	std::vector<Vector3> featurePoints;

	float InvertNoise(float value) { return 1 - Clamp(Remap(value, 0.f, 1.2f, 0.f, 1.f), 0.f, 1.f); }
};