#include "GeneralFrameBuffer.h"

const GLenum GeneralFrameBuffer::draw_buffers[] = { GL_COLOR_ATTACHMENT0 };

GeneralFrameBuffer::GeneralFrameBuffer(int width, int height)
{
	//Generate color texture
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Generate depth texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);

	//Generate framebuffer object and bind it with two textures above;
	glCreateFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	//Specify the order of rendering in color attachment, via an array of GL enum. 
	glDrawBuffers(1, draw_buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GeneralFrameBuffer::~GeneralFrameBuffer()
{
	glDeleteTextures(1, &colorTexture);
	glDeleteTextures(1, &depthTexture);
	glDeleteFramebuffers(1, &frameBuffer);
}