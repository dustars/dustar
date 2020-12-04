#include "HeightMap.h"

#include <iostream>
#include <fstream>
#include <random>
#include <time.h>
#include "ComputeShader.h"

HeightMap::HeightMap(int layers, double lacunarity, double gain, int width, int length) : Mesh(){
	
	numOfVertices = width * length;
	numOfIndex = (width - 1) * (length - 1) * 6;

	Vector2 centerOffset = Vector2(HEIGHTMAP_X * width * 0.5, HEIGHTMAP_Z * length * 0.5);

	GLuint noiseBuffer = GenerateNoiseBuffer(width, length, layers, lacunarity, gain); // <6ms!!!
	float* noisePtr = static_cast<float*>(glMapNamedBufferRange(noiseBuffer, 0, sizeof(float) * width * length, GL_MAP_READ_BIT));

	//Generate Per-Vertex attributes
	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < length; ++z) {

			float n = noisePtr[x * width + z];
			n = n * 1000 - 300;

			position.push_back(Vector3( x * HEIGHTMAP_X - centerOffset.x, static_cast<float>(n), z * HEIGHTMAP_Z - centerOffset.y));
			GenerateHeightColor(n);
			texCoord.push_back(Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z));
		}
	}

	glUnmapNamedBuffer(noiseBuffer);
	glDeleteBuffers(1, &noiseBuffer);

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

GLuint HeightMap::GenerateNoiseBuffer(size_t width, size_t height, int layers, float lacunarity, float gain,
									  size_t maxTableSize, float samplingRate)
{
	//TODO: Randomly shuffle the permutation table.
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

	ComputeShader perlinNoiseCS("shader/ComputeShader/PerlinNoiseBufferCS.glsl");
	glUseProgram(perlinNoiseCS.GetProgram());

	//Random value size and the rate of sampling
	glUniform1ui(0, maxTableSize);
	glUniform1f(1, samplingRate);
	glUniform1i(2, layers);
	glUniform1f(3, lacunarity);
	glUniform1f(4, gain);

	GLuint permutationInput;
	glCreateBuffers(1, &permutationInput);
	glNamedBufferStorage(permutationInput, sizeof(permutationTable), (const void*)permutationTable, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, permutationInput);

	//noise buffer
	GLuint noiseBuffer;
	glCreateBuffers(1, &noiseBuffer);
	glNamedBufferStorage(noiseBuffer, sizeof(float) * width * height, NULL, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, noiseBuffer);

	//Execute
	glDispatchCompute(ceil(width / 8), ceil(height / 8), 1);

	glUseProgram(0);

	return noiseBuffer;
}
