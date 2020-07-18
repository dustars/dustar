/*
	Description:
	Base class for framebuffer object.

	Is this base class really needed?
	Even if more types of FBO will be used in the future, each one of them will be unique,
	which means it's unncessary to use inheritance, and also no places in this application
	will need the base class as the pointer type in order to support polymorphism
*/


#pragma once
#include <GL/glew.h>

class FrameBuffer
{
public:
	virtual ~FrameBuffer() = 0 {}

	virtual GLuint GetFrameBuffer()  const = 0;
	virtual GLuint GetColorTexture() const = 0;
	virtual GLuint GetDepthTexture() const = 0;
};

