#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/stb_image.h"

bool Texture::SetTexture(std::string file, unsigned numR)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(file.c_str(), &width, &height, &nChannels, 0);

	int GL_Channels = GL_RGB;
	if (nChannels == 3) {
		GL_Channels = GL_RGB;
	}
	else if (nChannels == 4) {
		GL_Channels = GL_RGBA;
	}

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_Channels, width, height, 0, GL_Channels, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);
	numOfRows = numR;
	return data ? true : false;
}
