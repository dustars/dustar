#pragma once
#include "Texture.h"
#include "Perlin.h"

class Cloud
{
public:
	Cloud();
	~Cloud();

private:
	GLuint framebuffer;
	GLuint texture;
};

