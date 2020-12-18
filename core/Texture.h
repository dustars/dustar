#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>

class Texture
{
public:
	enum class TextureType {
		DEFAULT,
		DIFFUSE,
		SPECULAR
	};

	Texture() : texture(0), type(TextureType::DEFAULT), numOfRows(0) {}
	~Texture() {}

	bool SetTexture(const std::string& file, unsigned numR = 1);
	void SetTexture(const char* right, const char* left, const char* top, const char* bottom, const char* back, const char* front);
	void SetTextureType(TextureType type) { this->type = type; }
	void SetTexture(GLuint t, unsigned numR = 1) { texture = t; numOfRows = numR; }

	GLuint GetTexture() { return texture; }
	const std::string& GetPath() { return path; }
	TextureType GetTextureType() { return type; }
	unsigned GetNumOfRows() { return numOfRows; }

private:
	GLuint texture;
	TextureType type;
	std::string path;
	unsigned numOfRows; //For texture altas
};

