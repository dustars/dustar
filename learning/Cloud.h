#pragma once
#include "Texture.h"

class Cloud
{
public:
	Cloud();
	~Cloud();

private:
	GLuint framebuffer;
	GLuint texture;
};

