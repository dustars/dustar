#include "ComputeShaderExamples.h"
#include <random>
#include <algorithm>
#include <stb_image.h>
#include <stb_image_write.h>

//Originally from the example provided by OpenGL superbible Chapter 10 Compute Shader
void InversePicture()
{
	ComputeShader test("shader/ComputeShader/InversePictureCS.glsl");
	glUseProgram(test.GetProgram());

	//Texture creation (for both I/O)
	GLuint readWriteTex;
	glGenTextures(1, &readWriteTex);
	glBindTexture(GL_TEXTURE_2D, readWriteTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;
	unsigned char* dataIn = stbi_load("../assets/Textures/container.png", &width, &height, &nChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataIn);
	stbi_image_free(dataIn);

	//Bind texture
	glBindImageTexture(0, readWriteTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

	//Execute compute shader
	glDispatchCompute(width / 32, height / 32, 1); //work group is 32

	SaveImageToFile(readWriteTex, width, height, 4);

	glUseProgram(0);
}

//Originally from the example provided by OpenGL superbible Chapter 10 Compute Shader
void PrefixSum()
{
	ComputeShader test("shader/ComputeShader/PrefixSumCS.glsl");
	glUseProgram(test.GetProgram());

	const size_t size = 2048;
	float data[size];
	for (size_t i = 0; i < size; i++) data[i] = 1;

	GLuint inputData;
	glCreateBuffers(1, &inputData);
	glNamedBufferStorage(inputData, sizeof(data), (const void*)data, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputData);

	GLuint outputData;
	glCreateBuffers(1, &outputData);
	glNamedBufferStorage(outputData, sizeof(data), (const void*)data, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputData);

	glDispatchCompute(1, 1, 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputData);
	float* ptr = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(data), GL_MAP_READ_BIT);
	
	for (size_t i = 0; i < size; i++) std::cout << ptr[i] << " ";
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glUseProgram(0);
}

//Originally from the example provided by OpenGL superbible Chapter 10 Compute Shader
void SummedAreaTable()
{
	ComputeShader test("shader/ComputeShader/PrefixSum2DCS.glsl");
	glUseProgram(test.GetProgram());

	GLuint inputImage;
	glGenTextures(1, &inputImage);
	glBindTexture(GL_TEXTURE_2D, inputImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;
	unsigned char* dataIn = stbi_load("../assets/Textures/container.png", &width, &height, &nChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataIn);
	stbi_image_free(dataIn);

	glBindImageTexture(0, inputImage, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

	//Run compute shader for both row and column
	glDispatchCompute(width, 1, 1);
	glDispatchCompute(height, 1, 1);

	SaveImageToFile(inputImage, width, height, 4);

	glUseProgram(0);
}

void ValueNoise2D()
{
	const int width	= 512;
	const int height	= 512;
	const size_t maxTableSize = 256;
	const float samplingRate = 0.05f;

	ComputeShader valueNoiseCS("shader/ComputeShader/ValueNoiseCS.glsl");
	glUseProgram(valueNoiseCS.GetProgram());

	//Random value size and the rate of sampling
	glUniform1ui(0, maxTableSize);
	glUniform1f(1, samplingRate);

	//Random Values
	float randomValues[maxTableSize];
	GenerateRandomValues(randomValues, maxTableSize);

	GLuint randomValuesInput;
	glCreateBuffers(1, &randomValuesInput);
	glNamedBufferStorage(randomValuesInput, sizeof(randomValues), (const void*)randomValues, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, randomValuesInput);

	//Permutation Table
	unsigned permutationTable[maxTableSize * 2];
	GeneratPermutationTable(permutationTable, maxTableSize);

	GLuint permutationInput;
	glCreateBuffers(1, &permutationInput);
	glNamedBufferStorage(permutationInput, sizeof(permutationTable), (const void*)permutationTable, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, permutationInput);

	//Output image
	GLuint resultImg;
	glGenTextures(1, &resultImg);
	glBindTexture(GL_TEXTURE_2D, resultImg);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindImageTexture(0, resultImg, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
	
	//Execute Compute Shader
	glDispatchCompute(ceil(width/8), ceil(height/8), 1);

	SaveImageToFile(resultImg, width, height, 4);
	
	glUseProgram(0);
}

void ValueNoise3D()
{
}

void PerlinNoise()
{
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

	const int width = 512;
	const int height = 512;
	const size_t maxTableSize = 256;
	const float samplingRate = 0.02f;

	ComputeShader perlinNoiseCS("shader/ComputeShader/PerlinNoiseCS.glsl");
	glUseProgram(perlinNoiseCS.GetProgram());

	//Random value size and the rate of sampling
	glUniform1ui(0, maxTableSize);
	glUniform1f(1, samplingRate);

	GLuint permutationInput;
	glCreateBuffers(1, &permutationInput);
	glNamedBufferStorage(permutationInput, sizeof(permutationTable), (const void*)permutationTable, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, permutationInput);

	//Output image
	GLuint resultImg;
	glGenTextures(1, &resultImg);
	glBindTexture(GL_TEXTURE_2D, resultImg);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindImageTexture(0, resultImg, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

	//Execute Compute Shader
	glDispatchCompute(ceil(width / 8), ceil(height / 8), 1);
	//SaveImageToFile(resultImg, width, height, 4);

	glUseProgram(0);
}

void PerlinNoiseBuffer()
{
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

	const int width = 512;
	const int height = 512;
	const size_t maxTableSize = 256;
	const float samplingRate = 0.02f;

	ComputeShader perlinNoiseCS("shader/ComputeShader/PerlinNoiseBufferCS.glsl");
	glUseProgram(perlinNoiseCS.GetProgram());

	//Random value size and the rate of sampling
	glUniform1ui(0, maxTableSize);
	glUniform1f(1, samplingRate);

	GLuint permutationInput;
	glCreateBuffers(1, &permutationInput);
	glNamedBufferStorage(permutationInput, sizeof(permutationTable), (const void*)permutationTable, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, permutationInput);


	GLuint noiseBuffer;
	glCreateBuffers(1, &noiseBuffer);
	glNamedBufferStorage(noiseBuffer, sizeof(float) * width * height, NULL, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, noiseBuffer);

	//Execute Compute Shader
	glDispatchCompute(ceil(width / 8), ceil(height / 8), 1);

	glUseProgram(0);
}

void WorleyNoise()
{
}

void GenerateRandomValues(float* data, size_t size, int seed)
{
	std::default_random_engine e(seed);
	std::uniform_real_distribution<float> u(0, 1);

	for (size_t i = 0; i < size; i++) {
		data[i] = u(e);
	}
}

void GeneratPermutationTable(unsigned* data, size_t size, int seed)
{
	std::default_random_engine e(seed);

	//shuffle the first half of the permutation table
	//And copy the first half to the second half
	for (auto i = 0; i < size; i++) {
		data[i] = i;
	}

	for (auto i = size - 1; i > 0; --i) {
		std::uniform_int_distribution<decltype(i)> d(0, i);
		std::swap(data[i], data[d(e)]);
		data[i + size] = data[i];
	}
	data[size] = data[0]; //Manually set the first value of the second half
}

void SaveImageToFile(GLuint texture, int width, int height, int channels)
{
	unsigned char* data = new unsigned char[width * height * 4];
	glGetTextureImage(texture, 0, GL_RGBA, GL_UNSIGNED_BYTE, width * height * 4, data);
	stbi_write_png("../tempResults/tempResult.png", width, height, 4, data, 0);
	delete[] data;
}
