#pragma once
#include <iostream>
#include "glew.h"
#include <string>


#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/stb_image.h"


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

