/*
	Description:
	Cloud model class.
	It contains several control parameters to guide the noise generation
	process, and 4 threads are used to spead up the generation (clearly not as fast as GPU)

	Once the control parameters are set up in the creation of model and they
	should not be changed, except for debug purpose.

	Todo:
	1. Optimizations for lambda to which the threads call?
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
	Cloud(std::size_t res_Base = 128, std::size_t res_Detail = 32, std::size_t res_WM = 512);
	~Cloud();

	GLuint GetBaseShapeTex() { return baseShapeTex; }
	GLuint GetDetailShapeNoiseTex() { return detailShapeTex; }
	GLuint GetWeatherMapTex() { return weatherMapTex; }
	GLuint GetBlueNoiseTex() { return blueNoiseTex; }

	void Update(float msec);

	//Don't bother the data encapsulation for these parameters. :)
	float globalCoverage = 0.8f;
	float globalDensity = 1.0f;
	float cloudScale = 0.7f;
	float cloudOffset = 0.f;

	float cloudHeightAboveGround = 2500.f; //unit: meter
	float cloudLayerLength = 2000.f;

	//Reference:  A. Kokhanovsky, (2003). Optical properties of terrestrial clouds. doi:10.1016
	float extinctionCoefficient = 0.01; //range 0.005 ~ 0.1

	//Warning: changing the number of samples will dramatically affect the performance!
	int sampleSteps = 64;
	int lightSampleSteps = 4;
	float firstRayMarchingFactor = 1.0f;
	float secondRayMarchingFactor = 1.0f;

private:
	void CreateBaseShapeTexture();
	void CreateBaseShapeTextureCS();
	void CreateDetailShapeTexture();
	void CreateDetailShapeTextureCS();
	void CreateWeatherMapTexture();
	void CreateBlueNoiseTexture();

	GLuint baseShapeTex;
	GLuint detailShapeTex;
	GLuint weatherMapTex;
	GLuint blueNoiseTex;

	float CreatePerlinWorleyNoise(std::size_t x, std::size_t y, std::size_t z );

	//Resolution
	std::size_t res_B;			//resolution for base shape texture
	std::size_t res_D;		//for detail shape texture
	std::size_t res_W;	//for weather map
	//Random seed number to generate noise
	int seed = 0;

	PerlinNoise perlin;
	WorleyNoise worley;
};

} //namespace atmosphere