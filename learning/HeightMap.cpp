#include "HeightMap.h"

#include <iostream>
#include <fstream>
#include <random>
#include <time.h>

HeightMap::HeightMap(int octave, double lacunarity, double persistence, int width, int length) : Mesh(){
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

			double n = noise.octavePerlin(i, j, 0.5, octave, lacunarity, persistence) * 3000;

			n -= 1500.0;

			position.push_back(Vector3(x * HEIGHTMAP_X, n, z * HEIGHTMAP_Z));

			Vector3 tempColor;
			if (n > 100) {
				if (n > 550.0f) { // Snow
					tempColor = { 1.f, 0.98f, 0.98f };
				}
				else if (n > 300.0f) { // Grass
					tempColor = { 0.133f, 0.545f, 0.133f };
				}
				else { // ground
					tempColor = { 0.1f, 1.0f, 0.498f };
				}
			}
			else {
				if (n < 0) { // water
					tempColor = { 0.125f, 0.698f, 0.667f };
				}
				else { // ground
					tempColor = { 0.545f, 0.271f, 0.075f };
				}
			}
			color.push_back(tempColor);
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
	GenerateNormals();
	BufferDataToGPU();
}