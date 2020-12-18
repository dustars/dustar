#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <GL/glew.h>

class ComputeShader
{
public:
	ComputeShader(const std::string&);
	~ComputeShader() { glDeleteProgram(program); }

	GLuint GetProgram() { return program; }

private:
	GLuint program = 0;

	bool LoadShaderFile(const std::string& file, std::string& data);
};

