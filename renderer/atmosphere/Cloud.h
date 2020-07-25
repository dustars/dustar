/*
	Description:
	Cloud model class.
	It contains several control parameters to guild the noise generation
	process, and 4 threads are used to spead up the generation. 

	Created:		5/17/2020
	Last udpated:	7/24/2020

	Todo:
	1. Optimizations for lambda which the threads call?
*/


#pragma once
#include "../WorleyNoise.h"
#include "../PerlinNoise.h"
#include <GL/glew.h>
#include <vector>


namespace atmosphere {

class Cloud
{
public:
	Cloud(std::size_t resolution = 128, bool isSavetoFile = false);
	~Cloud();

	GLuint GetBaseShapeTex() { return baseShapeTex; }
	GLuint GetDetailShapeNoiseTex() { return detailShapeTex; }

private:
	void CreateBaseShapeTexture();
	void CreateDetailShapeTexture();
	void CreateWeatherMapTexture();

	GLuint baseShapeTex;
	GLuint detailShapeTex;
	GLuint weatherMapTex;

	float CreatePerlinWorleyNoise(std::size_t x, std::size_t y, std::size_t z );

	//Resolution
	std::size_t res_B; //resolution for base shape texture
	std::size_t res_D = 32; //for detail shape texture
	//Random seed number to generate noise
	int seed = 0;

	PerlinNoise perlin;
	WorleyNoise worley;

	float globalCoverage;
	float globalDensity;

	//temp
	bool isSavetoFile = false;
	void CreateNoiseImageSlice();
};

} //namespace atmosphere