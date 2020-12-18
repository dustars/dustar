#pragma once
#include "ComputeShader.h"

void InversePicture();
void PrefixSum();
void SummedAreaTable();
void ValueNoise2D();
void ValueNoise3D();
void PerlinNoise();
void PerlinNoiseBuffer();
void WorleyNoise();


//helper
void GenerateRandomValues(float* data, size_t size, int seed = 0);
void GeneratPermutationTable(unsigned* data, size_t size, int seed = 0);

//ONLY FOR PNG FORMAT
void SaveImageToFile(GLuint texture, int width, int height, int channels);