//cout << glGetString(GL_VERSION) << endl;
//cout << glGetString(GL_VENDOR) << endl;
//cout << glGetString(GL_RENDERER) << endl;
//cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

//void Renderer::CreateComputeShader() {
//	GLuint compute_shader;
//
//	static const GLchar* compute_shader_source[] =
//	{
//		"#version 450 core												\n"
//		"																\n"
//		"layout (local_size_x = 32, local_size_y = 32) in;				\n"
//		"																\n"
//		"void main(void)												\n"
//		"{																\n"
//		" // Do nothing													\n"
//		"}																\n"
//	};
//
//	compute_shader = glCreateShader(GL_COMPUTE_SHADER);
//	glShaderSource(compute_shader, 1, compute_shader_source, NULL);
//	glCompileShader(compute_shader);
//
//	computeProgram = glCreateProgram();
//	glAttachShader(computeProgram, compute_shader);
//	glLinkProgram(computeProgram);
//	glDeleteShader(compute_shader);
//}