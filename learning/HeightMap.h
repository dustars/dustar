#pragma once

#include <string>

#include "Mesh.h"
#include "Perlin.h"

#define HEIGHTMAP_X 8.0f
#define HEIGHTMAP_Z 8.0f

#define HEIGHTMAP_TEX_X 1.0f/16.0f
#define HEIGHTMAP_TEX_Z 1.0f/16.0f

class HeightMap : public Mesh {
public:
	HeightMap(int octave, double lacunarity, double persistence, int width = 100, int length = 100);  // Random Generation by using Perlin Noise
	HeightMap(std::string name);				 // Read from file, TODO in the future;

	~HeightMap(void) {};
};


