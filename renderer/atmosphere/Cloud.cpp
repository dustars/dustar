#include "Cloud.h"
#include "../Configuration.h"
#include "Utility.h"
#include <thread>

namespace atmosphere {

//Utilities


Cloud::Cloud(std::size_t resolution, bool isSavetoFile) :
	res(resolution),
	perlin(PerlinNoise(seed)),
	worley(WorleyNoise(res, 8, seed)),
	isSavetoFile(isSavetoFile)
{
	CreateBaseShapeTexture();
	CreateDetailShapeTexture();
	CreateWeatherMapTexture();
}

Cloud::~Cloud()
{
	glDeleteTextures(1, &baseShapeTex);
}


void Cloud::CreateBaseShapeTexture()
{
	//Create high frequency texture
	glGenTextures(1, &baseShapeTex);
	glBindTexture(GL_TEXTURE_3D, baseShapeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = new unsigned char[res * res * res];

#ifdef THREADING
	int numThreads = 4;
	std::vector<std::thread> tp;

	//lambda for thread to execute
	static auto NoiseCalculation = [&](std::size_t z) {
		for (std::size_t y = 0; y < res; y++) {
			for (std::size_t x = 0; x < res; x++) {
				data[z * res * res + y * res + x] = static_cast<unsigned char>(CreatePerlinWorleyNoise(x, y, z) * 255);
			}
		}
	};

	//Each thread will execute res * res * (res/numThreads) amount of noise data,
	//which means the whole 3D texture is equally divided and each part is executed
	//by a single thread.
	for (std::size_t z = 0; z < res / numThreads; z++) {
		for (std::size_t i = 0; i < numThreads; i++) tp.push_back(std::thread(NoiseCalculation, z * numThreads + i));
		for (std::size_t i = 0; i < numThreads; i++) tp[i].join();
		tp.clear();
	}

#else //If not multi-threading, an n^3 loop will do the job.
	for (std::size_t z = 0; z < res; z++) {
		for (std::size_t y = 0; y < res; y++) {
			for (std::size_t x = 0; x < res; x++) {
				data[z * res * res + y * res + x] = static_cast<unsigned char>(CreatePerlinWorleyNoise(x, y, z) * 255);
			}
		}
	}
#endif

	//Update the texture with the noise data
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, res, res, res, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<void*>(data));

	if (isSavetoFile) SaveAsPicture("../demo/PerlinWorleyNoise.jpg", res, res, 1, static_cast<void*>(data));

	delete[] data;
}

void Cloud::CreateDetailShapeTexture()
{
}

void Cloud::CreateWeatherMapTexture()
{
}

float Cloud::CreatePerlinWorleyNoise(std::size_t x, std::size_t y, std::size_t z)
{
	float worleyNoise = worley.FBMNoise(x, y, z, 3, 2.f, 0.707f);
	float perlinNoise = perlin.FBMPerlin(
		static_cast<double>(x) / res * 3.0,
		static_cast<double>(y) / res * 3.0,
		static_cast<double>(z) / res * 3.0,
		7, 2, 0.707);

	perlinNoise = Clamp(Remap(perlinNoise, 0.0, 1.0, -0.3f, 1.0f), 0.f, 1.f);

	return Remap(perlinNoise, 1.f - worleyNoise, 1.f, 0.f, 1.f);
}


} //namespace atmosphere