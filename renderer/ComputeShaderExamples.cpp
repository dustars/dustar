#include "ComputeShaderExamples.h"
#include <stb_image.h>
#include <stb_image_write.h>

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
	unsigned char* dataIn = stbi_load("../assets/Textures/particleStar.png", &width, &height, &nChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataIn);
	stbi_image_free(dataIn);
	//Bind texture
	glBindImageTexture(0, readWriteTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

	//Execute compute shader
	glDispatchCompute(width / 32, height / 32, 1); //work group is 32

	//Save texture to file
	unsigned char* dataOut = new unsigned char[width * height * 4];
	glGetTextureImage(readWriteTex, 0, GL_RGBA, GL_UNSIGNED_BYTE, width * height * 4, dataOut);
	stbi_write_png("../demo/computeShader.png", width, height, 4, dataOut, 0);
}

void PrefixSum()
{
	ComputeShader test("shader/ComputeShader/PrefixSumCS.glsl");
	glUseProgram(test.GetProgram());

	float data[1024];
	for (auto& ele : data) ele = 1.f;

	GLuint inputData;
	glCreateBuffers(1, &inputData);
	glNamedBufferStorage(inputData, sizeof(data), (const void*)data, GL_DYNAMIC_STORAGE_BIT);

	GLuint ouputData;
	glCreateBuffers(1, &ouputData);
	glNamedBufferStorage(ouputData, sizeof(data), NULL, GL_DYNAMIC_STORAGE_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputData);

}
