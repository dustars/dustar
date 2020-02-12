#include "Shader.h"
#include <iostream>

Shader::Shader(bool gs, bool ts) {
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	std::cout << "Vertex Shader created!" << std::endl;

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	std::cout << "Fragment Shader created!" << std::endl;

	control_shader = 0;
	evaluation_shader = 0;
	geometry_shader = 0;

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	if (gs) {
		CreateGS();
		std::cout << "Geometry Shader created!" << std::endl;
	}

	if (ts) {
		CreateTS();
		std::cout << "Control and Evaluation Shader created!" << std::endl;
	}

	glLinkProgram(program);
}

Shader::~Shader() {
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	if (geometry_shader) {
		glDeleteShader(geometry_shader);
	}
	if (control_shader) {
		glDeleteShader(control_shader);
	}
	if (evaluation_shader) {
		glDeleteShader(evaluation_shader);
	}
	glDeleteProgram(program);
	std::cout << "\nShader is deleted!" << std::endl;
}

void Shader::CreateGS() {
	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader, 1, geometry_shader_source, NULL);
	glCompileShader(geometry_shader);
	glAttachShader(program, geometry_shader);
}
void Shader::CreateTS() {
	control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(control_shader, 1, control_shader_source, NULL);
	glCompileShader(control_shader);
	glAttachShader(program, control_shader);

	evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(evaluation_shader, 1, evaluation_shader_source, NULL);
	glCompileShader(evaluation_shader);
	glAttachShader(program, evaluation_shader);
}