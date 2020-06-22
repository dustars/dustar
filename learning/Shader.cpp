#include "Shader.h"

Shader::Shader(string vs, string fs, string gs, string cs, string es) {
	program = glCreateProgram();

	vertex_shader = GenerateShader(vs, GL_VERTEX_SHADER);
	glAttachShader(program, vertex_shader);

	fragment_shader = GenerateShader(fs, GL_FRAGMENT_SHADER);
	glAttachShader(program, fragment_shader);

	control_shader = 0;
	evaluation_shader = 0;
	geometry_shader = 0;

	if (!gs.empty()) {
		CreateGS(gs);
	}

	if (!cs.empty() && !es.empty()) {
		CreateTS(cs, es);
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
}

void Shader::CreateGS(string gs) {
	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader, 1, 0, NULL);
	glCompileShader(geometry_shader);
	glAttachShader(program, geometry_shader);
}
void Shader::CreateTS(string cs, string es) {
	control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(control_shader, 1, 0, NULL);
	glCompileShader(control_shader);
	glAttachShader(program, control_shader);

	evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(evaluation_shader, 1, 0, NULL);
	glCompileShader(evaluation_shader);
	glAttachShader(program, evaluation_shader);
}

bool Shader::LoadShaderFile(string from, string& into) {

	ifstream file;
	string temp;

	file.open(from.c_str());
	if (!file.is_open()) {
		cout << " File does not exist !" << endl;
		return false;
	}
	while (!file.eof()) {
		getline(file, temp);
		into += temp + "\n";
	}

	file.close();
	cout << "Loading shader text from " << from << endl;
	return true;
}

GLuint Shader::GenerateShader(string from, GLenum type) {
	cout << "Compiling Shader ..." << endl;

	string load;
	if (!LoadShaderFile(from, load)) {
		cout << "Compiling failed!" << endl;
		loadFailed = true;
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status); //verify shader compilation

	if (status == GL_FALSE) {
		cout << "Compiling failed!" << endl;
		char error[512];
		glGetInfoLogARB(shader, sizeof(error), NULL, error);
		cout << error;
		loadFailed = true;
		return 0;
	}
	cout << "Compiling success!" << endl << endl;
	loadFailed = false;
	return shader;
}

bool Shader::LinkProgram() {
	if (loadFailed) {
		return false;
	}
	glLinkProgram(program);

	GLint code;

	glGetProgramiv(program, GL_LINK_STATUS, &code);
	return code == GL_TRUE ? true : false;
}