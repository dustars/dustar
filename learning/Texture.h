#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Perlin.h"
#include "glew.h"

using namespace std;

class Texture
{
public:
	Texture() : texture(0), numOfRows(0) {}
	~Texture() {}

	bool SetTexture(const string& file, unsigned numR = 1);
	void SetTexture(GLuint t, unsigned numR = 1) { texture = t; numOfRows = numR; }
	GLuint GetTexture() { return texture; }
	unsigned GetNumOfRows() { return numOfRows; }
	void CreateCubeMap(string right, string left, string top, string bottom, string back, string front);

private:
	GLuint texture;
	unsigned numOfRows; //For texture altas
};

