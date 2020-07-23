/*
	Description:
	A simple text renderer by using FreeType. Implementation followed by LearnOpenGL:
	https://learnopengl.com/In-Practice/Text-Rendering. Currently a arial TrueTpye
	font is by default the rendering text.

	Last Updated:
	2020/7/23
	change type of the field width and height into int.

	TODO:
	1.	Buffer the vertices once, and use matrices to scale and translate a glyph's quad.
		(can save 88% time of current rendering).
	2.	Now only support one font type, add more utilities if necessary.
	3.	The type of the field width and height in class character should be
		unsigned int, but in rendering negative value might be needed, so I
		temporarily change both into int type.
*/

#pragma once

#include <map>
#include <GL/glew.h>
#include "../math/Math.h"
#include "../Shader.h"

class TextRenderer
{
public:
	// Initialize the FreeType library and create glyphs
	TextRenderer(std::size_t width, std::size_t height, std::size_t fontSize = 16);
	~TextRenderer() {}

	void RenderText(const std::string& text, float x, float y, float scale = 1.f, Vector3 color = Vector3(1.f, 0, 0));

private:
	struct Character {
		GLuint textureID;  // ID handle of the glyph texture
		int width;
		int height;
		int bearingX;
		int bearingY;
		unsigned int advance;    // Offset to advance to next glyph
	};

	std::map<char, Character> Characters;

	//Shader, vao and vbo.
	GLuint text_vao;
	GLuint text_vbo;
	Shader textShader;
};

