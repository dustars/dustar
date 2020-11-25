#include "FrameBuffer.h"

const GLenum FrameBuffer::draw_buffers[] = { GL_COLOR_ATTACHMENT0 };

FrameBuffer::FrameBuffer(int width, int height, FBOCreationType type) :
	width(width), height(height), FBOtype(type)
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Create Textures
	switch (FBOtype)
	{
	case FBOCreationType::GENERAL:
		GeneralFBO();
		break;
	case FBOCreationType::DEPTH:
		DepthOnlyFBO();
		//GeneralFBO();
		break;
	case FBOCreationType::MULTIPLE:
		//placeholder();
		break;
	default:
		std::cout << "FBO Creation Failed: invalid type" << std::endl;
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	if(!colorTexture) glDeleteTextures(1, &colorTexture);
	if(!depthTexture) glDeleteTextures(1, &depthTexture);
	glDeleteFramebuffers(1, &frameBuffer);
}

void FrameBuffer::GeneralFBO()
{
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	//why this operation brings OpenGL error 1281
	//Because the level parameter has to be 1 to create at least one texture
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffers(1, draw_buffers);
}

void FrameBuffer::DepthOnlyFBO()
{
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);
}
