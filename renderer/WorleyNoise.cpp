#include "WorleyNoise.h"
#include <random>

WorleyNoise::WorleyNoise(std::size_t resolution, std::size_t cellsNums, int seed)
	: resolution(resolution),
	cellsNums(cellsNums)
{
	std::default_random_engine e(seed);
	std::uniform_real_distribution<float> u(0, 1);

	for (std::size_t i = 0; i < cellsNums * cellsNums * cellsNums; i++) {
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

	//Calculating distances in neighboring cubes, 3 x 3 x 3  = 27 in totals.
	float min = 2.f;
	int kk = 0, jj = 0, ii = 0;
	for (int i = -1; i <= 1; i++) {
		if (cZ + i == -1)			ii = cellsNums;
		if (cZ + i == cellsNums)	ii = 0 - cellsNums;
		for (int j = -1; j <= 1; j++) {
			if (cY + j == -1)			jj = cellsNums;
			if (cY + j == cellsNums)	jj = 0 - cellsNums;
			for (int k = -1; k <= 1; k++) {
				if (cX + k == -1)			kk = cellsNums;
				if (cX + k == cellsNums)	kk = 0 - cellsNums;
				Vector3 fp = featurePoints[(cZ + i + ii) * cellsNums * cellsNums + (cY + j + jj) * cellsNums + (cX + k + kk)];
				float temp = EulerDistance(x, y, z, fp.x + k, fp.y + j, fp.z + i);
				if (temp < min) min = temp;
				kk = 0;
			}
			jj = 0;
		}
		ii = 0;
	}

	return Clamp(1 - min, 0, 1);
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
