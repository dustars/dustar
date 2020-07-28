#pragma once
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

inline void SaveAsPicture(const char* filename, std::size_t width, std::size_t height, std::size_t comp,  const void* data)
{
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(filename, width, height, comp, data, 100);
}