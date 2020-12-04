#pragma once

#include "Mesh.h"

#define HEIGHTMAP_X 8.0f
#define HEIGHTMAP_Z 8.0f

#define HEIGHTMAP_TEX_X 1.0f/16.0f
#define HEIGHTMAP_TEX_Z 1.0f/16.0f

class HeightMap : public Mesh {
public:
	//Random Generation by using Perlin Noise
	HeightMap(int layers, double lacunarity = 2.0, double gain = 0.707, int width = 100, int length = 100);

	~HeightMap(void) {};

private:
	void GenerateHeightColor(double n);
	//Return the noise buffer
	GLuint GenerateNoiseBuffer(	size_t width, size_t height, int layers = 5, float lacunarity = 2.f, float gain = 0.707f,
								size_t maxTableSize = 256, float samplingRate = 0.005f );
};


