#include "ComputeShaderExamples.h"
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

	//Save texture to file
	unsigned char* dataOut = new unsigned char[width * height * 4];
	glGetTextureImage(readWriteTex, 0, GL_RGBA, GL_UNSIGNED_BYTE, width * height * 4, dataOut);
	stbi_write_png("../demo/computeShader.png", width, height, 4, dataOut, 0);
	delete[] dataOut;
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

	////save the image to a texture
	//unsigned char* dataOut = new unsigned char[width * height * nChannels];
	//glGetTextureImage(inputImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, width * height * nChannels, dataOut);
	//stbi_write_png("../demo/SAT.png", width, height, nChannels, dataOut, 0);
	//delete[] dataOut;
}
