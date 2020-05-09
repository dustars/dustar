#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "glew.h"

using namespace std;

class Texture
{
public:
	Texture() : texture(0), numOfRows(0) {}
	~Texture() {}

	bool SetTexture(string file, unsigned numR = 1);
	unsigned GetTexture() { return texture; }
	unsigned GetNumOfRows() { return numOfRows; }
	void CreateCubeMap(string right, string left, string top, string bottom, string back, string front);

private:
	unsigned texture;
	unsigned numOfRows; //For texture altas
};

