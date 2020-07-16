#pragma once

#include <string>

#include "Mesh.h"
#include "PerlinNoise.h"

#define HEIGHTMAP_X 8.0f
#define HEIGHTMAP_Z 8.0f

#define HEIGHTMAP_TEX_X 1.0f/16.0f
#define HEIGHTMAP_TEX_Z 1.0f/16.0f

class HeightMap : public Mesh {
public:
	//Random Generation by using Perlin Noise
	HeightMap(int octave, double lacunarity = 2.0, double gain = 0.707, int width = 100, int length = 100);

	~HeightMap(void) {};

private:
	void GenerateHeightColor(double n);
};


