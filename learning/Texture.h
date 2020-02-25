#pragma once
#include <iostream>
#include "glew.h"
#include <string>


class Texture
{
public:
	Texture() : texture(0), numOfRows(0) {}
	~Texture() {}

	bool SetTexture(std::string file, unsigned numR = 1);
	unsigned GetTexture() { return texture; }

	unsigned GetNumOfRows() { return numOfRows; }

private:
	unsigned texture;
	unsigned numOfRows; //For texture altas
};

