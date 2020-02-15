#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "glew.h"

using namespace std;

static const GLchar* control_shader_source[] =
{
	"#version 450 core									\n"
	"													\n"
	"layout (vertices = 3) out;							\n"
	"													\n"
	"void main(void)									\n"
	"{													\n"
	"	if (gl_InvocationID == 0)                       \n"
	"	{												\n"
	"		gl_TessLevelInner[0] = 5.0;                 \n"
	"		gl_TessLevelOuter[0] = 5.0;                 \n"
	"		gl_TessLevelOuter[1] = 5.0;                 \n"
	"		gl_TessLevelOuter[2] = 5.0;                 \n"
	"	}												\n"
	"													\n"
	"	gl_out[gl_InvocationID].gl_Position =           \n"
	"		gl_in[gl_InvocationID].gl_Position;			\n"
	"}													\n"
};

static const GLchar* evaluation_shader_source[] =
{
	"#version 450 core												\n"
	"																\n"
	"layout (triangles, equal_spacing, cw) in;						\n"
	"																\n"
	"void main(void)												\n"
	"{																\n"
	"	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +		\n"
	"				   gl_TessCoord.y * gl_in[1].gl_Position +		\n"
	"				   gl_TessCoord.z * gl_in[2].gl_Position);		\n"
	"}																\n"
};

static const GLchar* geometry_shader_source[] =
{
	"#version 450 core												\n"
	"																\n"
	"layout (triangles) in;											\n"
	"layout (points, max_vertices = 3) out;							\n"
	"																\n"
	"void main(void)												\n"
	"{																\n"
	"	int i;														\n"
	"	for (i = 0; i < gl_in.length(); i++)						\n"
	"	{															\n"
	"		gl_Position = gl_in[i].gl_Position;						\n"
	"		EmitVertex();											\n"
	"	}															\n"
	"}																\n"
};

class Shader {
public:
	Shader(string vs, string fs, string gs = "", string cs = "", string es = "");
	~Shader();

	inline const GLuint GetProgram() { return program; }
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

