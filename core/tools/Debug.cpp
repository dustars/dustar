#include "Debug.h"

void Debug::FBOCompletenessCheck()
{
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (fboStatus)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		std::cout << "Oops, no window exists?" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "Check the status of each attachment" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "Attach at least one buffer to the FBO" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "Check that all attachments enabled via glDrawBuffers exist in FBO" << std::endl;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "Check that the buffer specified via glReadBuffer exists in FBO" << std::endl;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "Reconsider formats used for attached buffers" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cout << "Make sure the number of samples for each attachment is the same" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cout << "Make sure the number of layers for each attachment is the same" << std::endl;
		break;
	}
}

void Debug::FormatCheck()
{
	GLint result;
	glGetInternalformativ(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, GL_DEPTH_RENDERABLE, 10, &result);

	if (result == GL_FALSE) {
		std::cout << "The depth texture is incomplete" << std::endl;
	}
	else {
		std::cout << "The depth texture is complete" << std::endl;
	}
}

void Debug::OpenGLStatusCheck()
{
	auto error = glGetError();
	if (error) {
		std::cout << "\nError(Code: " << error << ")."  << glewGetErrorString(error) << std::endl;
	}
}