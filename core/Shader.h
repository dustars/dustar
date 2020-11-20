#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>

using namespace std;

class Shader {
public:
	Shader(string vs = "", string fs = "", string gs = "", string cs = "", string es = "");
	~Shader();

	GLuint GetProgram() { return program; }
	bool LinkProgram();

protected:
	GLuint program;

	GLuint vertex_shader;
	GLuint control_shader;
	GLuint evaluation_shader;
	GLuint geometry_shader;
	GLuint fragment_shader;

	bool LoadShaderFile(string from, string& into);
	GLuint GenerateShader(string from, GLenum type);

	void CreateGS(string gs);
	void CreateTS(string cs, string es);

	bool loadFailed;
};

