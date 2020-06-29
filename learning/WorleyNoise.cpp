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

float WorleyNoise::noise(float x, float y, float z)
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

	std::vector<float> disArray;
	//Calculating distances in neighboring cubes, 3 x 3 x 3  = 27 in totals.
	for (int i = -1; i <= 1; i++) {
		if (cZ + i < 0 || cZ + i >= cellsNums) continue;
		for (int j = -1; j <= 1; j++) {
			if (cY + j < 0 || cY + j >= cellsNums) continue;
			for (int k = -1; k <= 1; k++) {
				if (cX + k < 0 || cX + k >= cellsNums) continue;
				Vector3 fp = featurePoints[(cZ + i) * cellsNums * cellsNums + (cY + j) * cellsNums + (cX + k)];
				disArray.push_back(EulerDistance(x, y, z, fp.x + k, fp.y + j, fp.z + i));
			}
		}
	}

	std::sort(disArray.begin(), disArray.end());
	return InvertNoise(disArray[0]);
}
