#include "WorleyNoise.h"
#include <random>

WorleyNoise::WorleyNoise(std::size_t resolution, std::size_t cellsNums, int seed)
	: resolution(resolution),
	cellsNums(cellsNums)
{
	std::default_random_engine e(seed);
	std::uniform_real_distribution<float> u(0, 1);

	for (int i = 0; i < cellsNums * cellsNums * cellsNums; i++) {
		featurePoints.push_back(Vector3(u(e), u(e), u(e)));
	}
}

float WorleyNoise::Noise(float x, float y, float z)
{
	//Relative position of point in a cube
	x /= (resolution / cellsNums);
	y /= (resolution / cellsNums);
	z /= (resolution / cellsNums);

	//which cube the point is in, mod the number of cells to ensure periodicity
	int cX = static_cast<int>(floor(x)) % cellsNums;
	int cY = static_cast<int>(floor(y)) % cellsNums;
	int cZ = static_cast<int>(floor(z)) % cellsNums;

	//Relative position of a point, from 0 to 1.
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	std::vector<float> disArray; //Store distance from feature point.
	//2x2x2 = 8 cubes searching, see Class description for detail.
	int i = floor(z - 0.5f);
	int j = floor(y - 0.5f);
	int k = floor(x - 0.5f);
	int ii = 0, jj = 0, kk = 0; //for noise repetition/mirror
	if (cZ + i < 0) ii = cellsNums;
	else if (cZ + i == cellsNums) ii = 0 - cellsNums;
	if (cY + j < 0) jj = cellsNums;
	else if (cY + j == cellsNums) jj = 0 - cellsNums;
	if (cX + k < 0) kk = cellsNums;
	else if (cX + k == cellsNums) kk = 0 - cellsNums;
	//Calculation of indices is a huge waste.
	Vector3 fp = featurePoints[cZ * cellsNums * cellsNums + cY * cellsNums + cX];
	disArray.push_back(EulerDistance(x, y, z, fp.x, fp.y, fp.z));
	fp = featurePoints[cZ * cellsNums * cellsNums + cY * cellsNums + (cX + k + kk)];
	disArray.push_back(EulerDistance(x, y, z, fp.x + k, fp.y, fp.z));
	fp = featurePoints[cZ * cellsNums * cellsNums + (cY + j + jj) * cellsNums + cX];
	disArray.push_back(EulerDistance(x, y, z, fp.x, fp.y + j, fp.z));
	fp = featurePoints[(cZ + i + ii) * cellsNums * cellsNums + cY * cellsNums + cX];
	disArray.push_back(EulerDistance(x, y, z, fp.x, fp.y, fp.z + i));
	fp = featurePoints[(cZ + i + ii) * cellsNums * cellsNums + (cY + j + jj) * cellsNums + cX];
	disArray.push_back(EulerDistance(x, y, z, fp.x, fp.y + j, fp.z + i));
	fp = featurePoints[(cZ + i + ii) * cellsNums * cellsNums + cY * cellsNums + (cX + k + kk)];
	disArray.push_back(EulerDistance(x, y, z, fp.x + k, fp.y, fp.z + i));
	fp = featurePoints[cZ * cellsNums * cellsNums + (cY + j + jj) * cellsNums + (cX + k + kk)];
	disArray.push_back(EulerDistance(x, y, z, fp.x + k, fp.y + j, fp.z));
	fp = featurePoints[(cZ + i + ii) * cellsNums * cellsNums + (cY + j + jj) * cellsNums + (cX + k + kk)];
	disArray.push_back(EulerDistance(x, y, z, fp.x + k, fp.y + j, fp.z + i));
	
	std::sort(disArray.begin(), disArray.end());
	return Clamp((1 - disArray[0]) * 1.5, 0.f, 1.f);
}

float WorleyNoise::FBMNoise(float x, float y, float z, std::size_t octaves, float lacunarity, float gain)
{
	float amplitude = 1.f, frequency = 1.f, worleyNoise = 0.f, maxValue = 0.f;
	for (std::size_t i = 0; i < octaves; i++) {
		worleyNoise += amplitude * Noise(x * frequency, y * frequency, z * frequency);
		maxValue += amplitude;

		frequency *= lacunarity;
		amplitude *= gain;
	}
	return worleyNoise / maxValue;
}
