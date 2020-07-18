#include "Cloud.h"

Cloud::Cloud()
{
	glCreateFramebuffers(1, &framebuffer);


	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGBA32F, 256, 256);
	glBindTexture(GL_TEXTURE_2D, texture);

}
