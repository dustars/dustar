#include "ComputeShader.h"

ComputeShader::ComputeShader(const std::string& file)
{
	program = glCreateProgram();
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

	std::cout << "Compiling Compute Shader ..." << std::endl;
	//Generate compute shader code string
	std::string data;
	assert(LoadShaderFile(file, data) && "Failed to load compute shader file!");
	const char* temp = data.c_str();
	glShaderSource(computeShader, 1, &temp, NULL);
	//Compile compute shader
	glCompileShader(computeShader);

	GLint status;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char error[512];
		glGetInfoLogARB(computeShader, sizeof(error), NULL, error);
		std::cout << error;
		assert(status == GL_TRUE && "Compiling failed!\n");
	}

	glAttachShader(program, computeShader);
	//linking
	glLinkProgram(program);
	GLint code;
	glGetProgramiv(program, GL_LINK_STATUS, &code);
	assert(code == GL_TRUE && "Compute Shader Linking failed!\n");

	glDeleteShader(computeShader);
	std::cout << "Compiling success!" << std::endl << std::endl;
}

bool ComputeShader::LoadShaderFile(const std::string& file, std::string& data) {
	std::ifstream f;
	std::string temp;

	f.open(file.c_str());
	if (!f.is_open()) {
		std::cout << " File does not exist !" << std::endl;
		return false;
	}
	while (!f.eof()) {
		getline(f, temp);
		data += temp + "\n";
	}

	f.close();
	std::cout << "Loading shader text from " << file << std::endl;
	return true;
}
