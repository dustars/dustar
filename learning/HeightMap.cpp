#include "HeightMap.h"

#include <iostream>
#include <fstream>
#include <random>
#include <time.h>

HeightMap::HeightMap(int octave, float persistence, int width, int length) : Mesh(){
	// Should have a setter for these values.
	
	numOfVertices = width * length;
	numOfIndex = (width - 1) * (length - 1) * 6;

	//Warning conversion
	srand(time(NULL));

	PerlinNoise noise(rand());

	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < length; ++z) {
			int offset = (x * width) + z;

			double i = (double)x / ((double)width);
			double j = (double)z / ((double)length);

			double n = noise.octavePerlin(10 * i, 10 * j, 0.8, octave, persistence) * 1000;


			position.push_back(Vector3(x * HEIGHTMAP_X, n/10, z * HEIGHTMAP_Z));
			texCoord.push_back(Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z));
		}
	}

	for (int x = 0; x < width - 1; ++x) {
		for (int z = 0; z < length - 1; ++z) {

			int a = (x * (width)) + z;
			int b = ((x + 1) * (width)) + z;
			int c = ((x + 1) * (width)) + (z + 1);
			int d = (x * (width)) + (z + 1);

			index.push_back(b);
			index.push_back(c);
			index.push_back(a);

			index.push_back(a);
			index.push_back(c);
			index.push_back(d);
		}
	}

	BufferDataToGPU();
}