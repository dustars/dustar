#pragma once
#include <iostream>
#include "glew.h"
#include <string>


class Texture
{
public:
	Texture() : texture(0) {}
	~Texture() {}

	bool SetTexture(std::string file);
	unsigned int GetTexture() { return texture; }
private:
	unsigned int texture;
};

