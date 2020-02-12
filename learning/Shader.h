#pragma once

#include "glew.h"

static const GLchar* vertex_shader_source[] =
{
	"#version 450 core															\n"
	"																			\n"
	"uniform mat4 modelViewMatrix;												\n"
	"uniform mat4 projMatrix;													\n"
	"																			\n"
	"layout (location = 0) in vec4 position;									\n"
	//"layout (location = 1) in vec4 color;										\n"
	"																			\n"
	"out VS{																	\n"
	"	vec4 color;																\n"
	"}OUT;																		\n"
	"																			\n"
	"void main(void)															\n"
	"{																			\n"
	"	gl_Position = projMatrix * modelViewMatrix * position;					\n"
	//"	gl_Position = position;													\n"
	//"	OUT.color = vec4(0.0, 0.0, 0.0, 1.0) + color;							\n"
	"	OUT.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);					\n"
	"}																			\n"
};

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

static const GLchar* fragment_shader_source[] =
{
	"#version 450 core									\n"
	"													\n"
	"in VS{												\n"
	"	vec4 color;										\n"
	"}IN;												\n"
	"													\n"
	"out vec4 color;									\n"
	"													\n"
	"void main(void)									\n"
	"{													\n"
	"  color = IN.color;								\n"
	"}													\n"
};

class Shader {
public:
	Shader(bool gs = false, bool bs = false);
	~Shader();

	inline const GLuint GetProgram() { return program; }

protected:
	GLuint program;

	GLuint vertex_shader;
	GLuint control_shader;
	GLuint evaluation_shader;
	GLuint geometry_shader;
	GLuint fragment_shader;

	void CreateGS();
	void CreateTS();
};

