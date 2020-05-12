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

	//Generate Per-Vertex attributes
	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < length; ++z) {
			double i = (double)x / ((double)width);
			double j = (double)z / ((double)length);

			//Perlin Noise
			double n = noise.octavePerlin(i, j, 0.5, octave, lacunarity, persistence) * 3000 - 1500;

			position.push_back(Vector3(x * HEIGHTMAP_X, n, z * HEIGHTMAP_Z));
			GenerateHeightColor(n);
			texCoord.push_back(Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z));
		}
	}

	//Generate indices
	for (int x = 0; x < width - 1; ++x) {
		for (int z = 0; z < length - 1; ++z) {

			int a = (x * (width)) + z;
			int b = (x * (width)) + (z + 1);
			int c = ((x + 1) * (width)) + z;
			int d = ((x + 1) * (width)) + (z + 1);
			// the order is clockwise, but back culling still works, Why? Do I get it wrong?
			index.push_back(a);
			index.push_back(b);
			index.push_back(c);

			index.push_back(b);
			index.push_back(d);
			index.push_back(c);
		}
	}
	GenerateNormals();
	BufferDataToGPU();
}

void HeightMap::GenerateHeightColor(double n)
{
	if (n > 100.0) {
		if (n > 550.0) { // Snow
			color.push_back(Vector3(1.f, 0.98f, 0.98f));
		}
		else if (n > 300.0) { // Grass
			color.push_back(Vector3(0.133f, 0.545f, 0.133f));
		}
		else { // ground
			color.push_back(Vector3(0.1f, 1.0f, 0.498f));
		}
	}
	else {
		if (n < 0.0) { // water
			color.push_back(Vector3(0.125f, 0.698f, 0.667f));
		}
		else { // ground
			color.push_back(Vector3(0.545f, 0.271f, 0.075f));
		}
	}
	
}
