#include "Cloud.h"
#include "../Configuration.h"
#include "ComputeShader.h"
#include "Utility.h"
#include <thread>
#include <random>

namespace atmosphere {

Cloud::Cloud(std::size_t res_Base, std::size_t res_Detail, std::size_t res_WM) :
	res_B(res_Base),
	res_D(res_Detail),
	res_W(res_WM),
	perlin(PerlinNoise(seed)),
	worley(WorleyNoise(res_B * 4, 32, seed))
{
	//CreateBaseShapeTexture();		//Take around 10s to calculate
	CreateBaseShapeTextureCS();
	//CreateDetailShapeTexture();
	CreateDetailShapeTextureCS();
	CreateWeatherMapTexture();
	CreateBlueNoiseTexture();
}

Cloud::~Cloud()
{
	glDeleteTextures(1, &baseShapeTex);
	glDeleteTextures(1, &detailShapeTex);
	glDeleteTextures(1, &weatherMapTex);
	glDeleteTextures(1, &blueNoiseTex);
}


void Cloud::CreateBaseShapeTexture()
{
	//Create high frequency texture
	glGenTextures(1, &baseShapeTex);
	glBindTexture(GL_TEXTURE_3D, baseShapeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = new unsigned char[res_B * res_B * res_B * 4];

#ifdef THREADING
	int numThreads = 4;
	std::vector<std::thread> tp;

	//lambda for thread to execute
	static auto NoiseCalculation = [&](std::size_t z) {
		for (std::size_t y = 0; y < res_B; y++) {
			for (std::size_t x = 0; x < res_B; x++) {
				data[(z * res_B * res_B + y * res_B + x) * 4    ] = static_cast<unsigned char>(CreatePerlinWorleyNoise(x, y, z) * 255);
				data[(z * res_B * res_B + y * res_B + x) * 4 + 1] = static_cast<unsigned char>(1 - worley.Noise(x * 0.5, y * 0.5, z * 0.5) * 255);
				data[(z * res_B * res_B + y * res_B + x) * 4 + 2] = static_cast<unsigned char>(1 - worley.Noise(x, y, z) * 255);
				data[(z * res_B * res_B + y * res_B + x) * 4 + 3] = static_cast<unsigned char>(1 - worley.Noise(x * 1.5, y * 1.5, z * 1.5) * 255);
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
				data[(z * res_B * res_B + y * res_B + x) * 4    ] = static_cast<unsigned char>(CreatePerlinWorleyNoise(x, y, z) * 255);
				data[(z * res_B * res_B + y * res_B + x) * 4 + 1] = static_cast<unsigned char>(1 - worley.Noise(x * 0.5, y * 0.5, z * 0.5) * 255);
				data[(z * res_B * res_B + y * res_B + x) * 4 + 2] = static_cast<unsigned char>(1 - worley.Noise(x, y, z) * 255);
				data[(z * res_B * res_B + y * res_B + x) * 4 + 3] = static_cast<unsigned char>(1 - worley.Noise(x * 1.5, y * 1.5, z * 1.5) * 255);
			}
		}
	}
#endif

	//Update the texture with the noise data
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, res_B, res_B, res_B, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<void*>(data));
	delete[] data;
}

void Cloud::CreateBaseShapeTextureCS()
{
	//Initilization of input data
	std::size_t numOfCells = 8u;
	std::vector<Vector3> featurePoints;
	std::default_random_engine e(seed);
	std::uniform_real_distribution<float> u(0, 1);
	for (std::size_t i = 0; i < numOfCells * numOfCells * numOfCells; i++) {
		featurePoints.push_back(Vector3(u(e), u(e), u(e)));
	}

	int permutationTable[] = {
	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
	8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
	35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
	134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
	55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
	18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
	250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
	189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
	43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
	97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
	107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
	//The Following repeats the first half of the table.
	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
	8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
	35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
	134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
	55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
	18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
	250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
	189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
	43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
	97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
	107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

	glGenTextures(1, &baseShapeTex);
	glBindTexture(GL_TEXTURE_3D, baseShapeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, res_B, res_B, res_B, 0, GL_RGBA, GL_FLOAT, NULL);

	//Compute Shader Creation
	ComputeShader BaseNoiseShader("shader/ComputeShader/CloudBaseTextureNoiseCS.glsl");
	glUseProgram(BaseNoiseShader.GetProgram());

	GLuint permutationInput;
	glCreateBuffers(1, &permutationInput);
	glNamedBufferStorage(permutationInput, sizeof(permutationTable), (const void*)permutationTable, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, permutationInput);

	GLuint featurePointsBuffer;
	glCreateBuffers(1, &featurePointsBuffer);
	glNamedBufferStorage(featurePointsBuffer, sizeof(featurePoints) * featurePoints.size(), (const void*)featurePoints.data(), GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, featurePointsBuffer);

	glBindImageTexture(0, baseShapeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	//Execute
	glDispatchCompute(ceil(res_B / 8), ceil(res_B / 8), res_B);

	glUseProgram(0);
}

void Cloud::CreateDetailShapeTexture()
{
	//Create high frequency texture
	glGenTextures(1, &detailShapeTex);
	glBindTexture(GL_TEXTURE_3D, detailShapeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	WorleyNoise worley1(res_D, 4, seed);
	WorleyNoise worley2(res_D, 8, seed);
	WorleyNoise worley3(res_D, 16, seed);

	unsigned char* data = new unsigned char[res_D * res_D * res_D * 3];

	for (std::size_t z = 0; z < res_D; z++) {
		for (std::size_t y = 0; y < res_D; y++) {
			for (std::size_t x = 0; x < res_D; x++) {
				data[(z * res_D * res_D + y * res_D + x) * 3] = static_cast<unsigned char>(1 - worley1.Noise(x, y, z) * 255);
				data[(z * res_D * res_D + y * res_D + x) * 3 + 1] = static_cast<unsigned char>(1 - worley2.Noise(x, y, z) * 255);
				data[(z * res_D * res_D + y * res_D + x) * 3 + 2] = static_cast<unsigned char>(1 - worley3.Noise(x, y, z) * 255);
			}
		}
	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, res_D, res_D, res_D, 0, GL_RGB, GL_UNSIGNED_BYTE, static_cast<void*>(data));
	delete[] data;
}

void Cloud::CreateDetailShapeTextureCS()
{
	//Initilization of input data
	std::size_t numOfCells = 4u;
	std::vector<Vector3> featurePoints;
	std::default_random_engine e(seed);
	std::uniform_real_distribution<float> u(0, 1);
	for (std::size_t i = 0; i < numOfCells * numOfCells * numOfCells; i++) {
		featurePoints.push_back(Vector3(u(e), u(e), u(e)));
	}

	glGenTextures(1, &detailShapeTex);
	glBindTexture(GL_TEXTURE_3D, detailShapeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, res_D, res_D, res_D, 0, GL_RGBA, GL_FLOAT, NULL);

	//Compute Shader Creation
	ComputeShader detailNoiseShader("shader/ComputeShader/CloudDetailTextureNoiseCS.glsl");
	glUseProgram(detailNoiseShader.GetProgram());

	GLuint featurePointsBuffer;
	glCreateBuffers(1, &featurePointsBuffer);
	glNamedBufferStorage(featurePointsBuffer, sizeof(featurePoints) * featurePoints.size(), (const void*)featurePoints.data(), GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, featurePointsBuffer);

	glBindImageTexture(0, detailShapeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	//Execute
	glDispatchCompute(ceil(res_D / 8), ceil(res_D / 8), res_D);

	glUseProgram(0);
}

void Cloud::CreateWeatherMapTexture()
{
	//Create high frequency texture
	glGenTextures(1, &weatherMapTex);
	glBindTexture(GL_TEXTURE_2D, weatherMapTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//stbi_set_flip_vertically_on_load(true);
	int width, height, nChannels;
	unsigned char* data = stbi_load("../assets/Textures/WeatherMapRedChannel_02.png", &width, &height, &nChannels, 0);
	assert(data != NULL && "Data not loaded by stbi_load");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_image_free(data);
}

void Cloud::CreateBlueNoiseTexture()
{
	glGenTextures(1, &blueNoiseTex);
	glBindTexture(GL_TEXTURE_2D, blueNoiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nChannels;
	unsigned char* data = stbi_load("../assets/Textures/blueNoise.png", &width, &height, &nChannels, 0);
	assert(data != NULL && "Blue Noise data loading fails");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

float Cloud::CreatePerlinWorleyNoise(std::size_t x, std::size_t y, std::size_t z)
{
	float worleyNoise = 1 - worley.FBMNoise(x, y, z, 3, 2.f, 0.707f);
	float perlinNoise = perlin.FBMPerlin(
		static_cast<double>(x) / res_B * 3.0f,
		static_cast<double>(y) / res_B * 3.0f,
		static_cast<double>(z) / res_B * 3.0f,
		3, 2, 0.707);

	worleyNoise = Clamp(Remap(worleyNoise, 0.1f, 0.9f, 0.f, 1.f), 0.f, 1.f);
	perlinNoise = Clamp(Remap(perlinNoise, 0.3f, 0.7f, 0.f, 1.1f), 0.f, 1.f);

	return Clamp(Remap(perlinNoise * worleyNoise, 0.f, 1.f, 0.2f, 1.7f), 0.f, 1.f);
}


} //namespace atmosphere