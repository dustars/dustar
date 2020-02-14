#include "ParticleRenderer.h"


ParticleRenderer::ParticleRenderer(Matrix4 projMatrix, Camera* camera) : camera(camera){

	createShader();

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo[BASEMESH_BUFFER]);

	static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
	};

	glNamedBufferStorage(vbo[BASEMESH_BUFFER], sizeof(g_vertex_buffer_data), g_vertex_buffer_data, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo[BASEMESH_BUFFER], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0); //?
	glVertexArrayAttribBinding(vao, 0, 0);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	glUseProgram(0);
}

ParticleRenderer::~ParticleRenderer()
{
	glDeleteVertexArrays(1, &vao);		//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, vbo);	//Delete our VBOs
}

void ParticleRenderer::RenderParticle()
{
	glUseProgram(program);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(false);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	Matrix4 viewMatrix = camera->BuildViewMatrix();
	for (auto&& element : Particle::ParticleList) {
		UpdateMatrix(element, viewMatrix);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
	}

	//glDepthMask(true);
	//glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void ParticleRenderer::UpdateMatrix(Particle& p, const Matrix4& viewMatrix)
{
	// iterate every particle and update their modelview matrix
	
	Matrix4 modelMatrix;
	modelMatrix = modelMatrix * modelMatrix.Translation(p.GetPosition());
	modelMatrix.values[0] = viewMatrix.values[0];
	modelMatrix.values[1] = viewMatrix.values[4];
	modelMatrix.values[2] = viewMatrix.values[8];
	modelMatrix.values[4] = viewMatrix.values[1];
	modelMatrix.values[5] = viewMatrix.values[5];
	modelMatrix.values[6] = viewMatrix.values[9];
	modelMatrix.values[8] = viewMatrix.values[2];
	modelMatrix.values[9] = viewMatrix.values[6];
	modelMatrix.values[10] = viewMatrix.values[10];

	modelMatrix = modelMatrix * modelMatrix.Rotation(DegToRad(p.GetRotation()), { 0,0,1 });
	modelMatrix = modelMatrix * modelMatrix.Scale({ p.GetScale(), p.GetScale(), p.GetScale() });
	Matrix4 modelViewMatrix = viewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&modelViewMatrix);
}

void ParticleRenderer::createShader()
{
	const GLchar* vertex_shader_source[] =
	{
		"#version 450 core															\n"
		"																			\n"
		"uniform mat4 modelViewMatrix;												\n"
		"uniform mat4 projMatrix;													\n"
		"																			\n"
		"layout (location = 0) in vec4 position;									\n"
		//"layout (location = 1) in vec4 color;										\n"
		//"																			\n"
		//"out VS{																	\n"
		//"	vec4 color;																\n"
		//"}OUT;																	\n"
		"																			\n"
		"void main(void)															\n"
		"{																			\n"
		"	gl_Position = projMatrix * modelViewMatrix * position;					\n"
		//"	gl_Position = position;													\n"
		//"	OUT.color = vec4(0.0, 0.0, 0.0, 1.0) + color;							\n"
		//"	OUT.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);					\n"
		"}																			\n"
	};

	const GLchar* fragment_shader_source[] =
	{
		"#version 450 core									\n"
		"													\n"
		//"in VS{											\n"
		//"	vec4 color;										\n"
		//"}IN;												\n"
		"													\n"
		"out vec4 color;									\n"
		"													\n"
		"void main(void)									\n"
		"{													\n"
		//"  color = IN.color;								\n"
		"  color = vec4(1.0);								\n"
		"}													\n"
	};

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	std::cout << "Vertex Shader created!" << std::endl;

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	std::cout << "Fragment Shader created!" << std::endl;

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
}
