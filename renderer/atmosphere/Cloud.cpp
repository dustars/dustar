#include "Cloud.h"
#include "../Configuration.h"
#include "Utility.h"
#include <thread>

namespace atmosphere {

Cloud::Cloud(std::size_t resolution, bool isSavetoFile) :
	res_B(resolution),
	perlin(PerlinNoise(seed)),
	worley(WorleyNoise(res_B, 8, seed)),
	isSavetoFile(isSavetoFile)
{
	//CreateNoiseImageSlice();
	CreateBaseShapeTexture();	
	CreateDetailShapeTexture();
	CreateWeatherMapTexture();
}

Cloud::~Cloud()
{
	glDeleteTextures(1, &baseShapeTex);
	glDeleteTextures(1, &detailShapeTex);
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

	unsigned char* data = new unsigned char[res_B * res_B * res_B];

#ifdef THREADING
	int numThreads = 4;
	std::vector<std::thread> tp;

	//lambda for thread to execute
	static auto NoiseCalculation = [&](std::size_t z) {
		for (std::size_t y = 0; y < res_B; y++) {
			for (std::size_t x = 0; x < res_B; x++) {
				data[z * res_B * res_B + y * res_B + x] = static_cast<unsigned char>(CreatePerlinWorleyNoise(x, y, z) * 255);
			}
		}
	};

	//Each thread will execute res * res * (res/numThreads) amount of noise data,
	//which means the whole 3D texture is equally divided and each part is executed
	//by a single thread.
	for (std::size_t z = 0; z < res_B / numThreads; z++) {
		for (std::size_t i = 0; i < numThreads; i++) tp.push_back(std::thread(NoiseCalculation, z * numThreads + i));
		for (std::size_t i = 0; i < numThreads; i++) tp[i].join();
		tp.clear();
	}

#else //If not multi-threading, an n^3 loop will do the job.
	for (std::size_t z = 0; z < res_B; z++) {
		for (std::size_t y = 0; y < res_B; y++) {
			for (std::size_t x = 0; x < res_B; x++) {
				data[z * res_B * res_B + y * res_B + x] = static_cast<unsigned char>(CreatePerlinWorleyNoise(x, y, z) * 255);
			}
		}
	}
#endif

	//Update the texture with the noise data
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, res_B, res_B, res_B, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<void*>(data));

	if (isSavetoFile) SaveAsPicture("../demo/PerlinWorleyNoise.jpg", res_B, res_B, 1, static_cast<void*>(data));

	delete[] data;
}

void Cloud::CreateDetailShapeTexture()
{
	//Create high frequency texture
	glGenTextures(1, &detailShapeTex);
	glBindTexture(GL_TEXTURE_3D, detailShapeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	WorleyNoise worley_D(res_D, 8, seed);

	unsigned char* data = new unsigned char[res_D * res_D * res_D * 3];

	for (std::size_t z = 0; z < res_D; z++) {
		for (std::size_t y = 0; y < res_D; y++) {
			for (std::size_t x = 0; x < res_D; x++) {
				data[ (z * res_D * res_D + y * res_D + x) * 3] = static_cast<unsigned char>(worley_D.FBMNoise(x, y, z, 3) * 255);
				data[ (z * res_D * res_D + y * res_D + x) * 3 + 1] = static_cast<unsigned char>(worley_D.FBMNoise(x*2, y*2, z*2, 3) * 255);
				data[ (z * res_D * res_D + y * res_D + x) * 3 + 2] = static_cast<unsigned char>(worley_D.FBMNoise(x*4, y*4, z*4, 3) * 255);
			}
		}
	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, res_D, res_D, res_D, 0, GL_RGB, GL_UNSIGNED_BYTE, static_cast<void*>(data));
	delete[] data;
}

void Cloud::CreateWeatherMapTexture()
{
}

float Cloud::CreatePerlinWorleyNoise(std::size_t x, std::size_t y, std::size_t z)
{
	float worleyNoise = worley.FBMNoise(x, y, z, 3, 2.f, 0.707f);
	float perlinNoise = perlin.FBMPerlin(
		static_cast<double>(x) / res_B * 3.0,
		static_cast<double>(y) / res_B * 3.0,
		static_cast<double>(z) / res_B * 3.0,
		3, 2, 0.707);

	perlinNoise = Remap(perlinNoise, 0, 1.2, 0, 1);

	return Clamp(Remap(perlinNoise, worleyNoise, 1.f, 0.f, 1.f), 0.f, 1.f);
}

void Cloud::CreateNoiseImageSlice()
{
	PerlinNoise perlinT(seed);
	WorleyNoise worleyT(256, 8, seed);

	unsigned char* data = new unsigned char[32 * 32];

	for (std::size_t y = 0; y < 32; y++) {
		for (std::size_t x = 0; x < 32; x++) {
			data[y * 32 + x] = static_cast<unsigned char>(worleyT.Noise(x, y, 0) * 255);
		}
	}
	SaveAsPicture("../demo/noise/HighFreqNoiseRedChannel.jpg", res_D, res_D, 1, static_cast<void*>(data));

	for (std::size_t y = 0; y < res_D; y++) {
		for (std::size_t x = 0; x < res_D; x++) {
			data[y * 32 + x] = static_cast<unsigned char>(worleyT.Noise(x * 1.5, y * 1.5, 0) * 255);
		}
	}
	SaveAsPicture("../demo/noise/HighFreqNoiseGreenChannel.jpg", res_D, res_D, 1, static_cast<void*>(data));

	for (std::size_t y = 0; y < res_D; y++) {
		for (std::size_t x = 0; x < res_D; x++) {
			data[y * 32 + x] = static_cast<unsigned char>(worleyT.Noise(x *2, y  *2, 0) * 255);
		}
	}
}


} //namespace atmosphere