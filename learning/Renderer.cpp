#include "Renderer.h"

Renderer::Renderer(Window& parent) : RenderBase(parent) {

	program = new Shader();

	CreateCube();
	//CreateTriangle();

	init = true;
}

Renderer::~Renderer(void) {
	glDeleteVertexArrays(1, &vertexArrayObject);
	delete program;
}

void Renderer::RenderScene() {
	const GLfloat backgroundColor[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
	glClearBufferfv(GL_COLOR, 0, backgroundColor);

	float f = (float)startingTime * (float)PI * 0.1f;
	Matrix4 modelMatrix =
		Matrix4::Translation(Vector3(0.0f, 0.0f, -4.0f)) *
		Matrix4::Translation(Vector3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f)) *
		Matrix4::Rotation((float)startingTime * 45.0f, Vector3(0.0f, 1.0f, 0.0f)) *
		Matrix4::Rotation((float)startingTime * 81.0f, Vector3(1.0f, 0.0f, 0.0f));

	Matrix4 projMatrix = Matrix4::Perspective(0.1f, 1000.0f, (float)width / (float)height, 50.0f);
	/*================================*/
	/*===========Run Shader===========*/
	/*================================*/
	glUseProgram(program->GetProgram());
	glBindVertexArray(vertexArrayObject);

	glUniformMatrix4fv(glGetUniformLocation(program->GetProgram(), "modelViewMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program->GetProgram(), "projMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	/// When the attribute is disabled, the vertex shader will be
	// provided with the static information you provide with a call to glVertexAttrib* ().
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	glUseProgram(0);
	::SwapBuffers(deviceContext);
}
// data through vertex attributes of VAO and data through VBO ?

void Renderer::CreateTriangle() {
	glCreateVertexArrays(1, &vertexArrayObject);
	//glGenVertexArrays(1, &vertexArrayObject);  // Note the difference between Gen and Create
	//glBindVertexArray(vertexArrayObject); //Why I need this?

	glCreateBuffers(MAXBUFFER, vertexBufferObject);
	//glGenBuffers(MAXBUFFER, vertexBufferObject);

	/*----------------------Allocate memory to the buffer----------------------//
		The fourth parameter in the following two buffer storage allocation functions is, flag, and it has the 6 types:
		(For further information, please refer to the OpenGL superbible 5th edition. Everthing is in there.)
		GL_DYNAMIC_STORAGE_BIT, if you need to update the buffer frequently.
		GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT, and GL_MAP_COHERENT_BIT, how you gonna access the buffer.
		GL_CILENT_STORAGE_BIT, I have no ideas
	*/
	// glBufferStorage(GL_ARRAY_BUFFER, numVertices*sizeof(Vector4), NULL, GL_DYNAMIC_STORAGE_BIT);
	// Note the difference here
	// Remember three ways to change the data in buffer (BufferSubData() and Map/Unmap)
	glNamedBufferStorage(vertexBufferObject[POSITION],	sizeof(position),	position,	0);
	glNamedBufferStorage(vertexBufferObject[COLOR],		sizeof(color),		color,		0);
	//glNamedBufferStorage(vertexBufferObject[TEXTURE],	sizeof(vertices),	vertices, 0);

	//// Get a pointer to the buffer's data store
	//void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//// Copy our data into it...
	//memcpy(ptr, data, sizeof(data));
	//// Tell OpenGL that we're done with the
	//glUnmapBuffer(GL_ARRAY_BUFFER);
	/* Alternatively, it's actually preferable to use glMapBufferRange() and glMapNamedBufferRange() */
	
	/* glClearBufferSubData() or glClearNamedBufferSubData(), for put constant value into buffer objects */
	/* glCopyBufferSubData() and glCopyNamedBufferSubData(), share exchange between buffers in GPU */

	//----------------------Bind buffer to OpenGL context----------------------// ????? Optional?
	//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[POSITION]);
	
	GLuint positionIndex =	glGetAttribLocation(program->GetProgram(), "position");
	GLuint colorIndex =		glGetAttribLocation(program->GetProgram(), "color");

	glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBufferObject[POSITION], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vertexArrayObject, positionIndex, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vertexArrayObject, positionIndex, 0);

	glVertexArrayVertexBuffer(vertexArrayObject, 1, vertexBufferObject[COLOR], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vertexArrayObject, colorIndex, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vertexArrayObject, colorIndex, 1);

	//glVertexArrayAttribBinding(vertexArrayObject, positionIndex, 0);
	//glVertexArrayAttribBinding(vertexArrayObject, colorIndex, 0);

	//glVertexArrayAttribFormat(vertexArrayObject, positionIndex, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
	//glVertexArrayAttribFormat(vertexArrayObject, colorIndex, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, r));

	//glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBufferObject[TEXTURE], 0, sizeof(vertex));
}

void Renderer::CreateCube() {
	glCreateVertexArrays(1, &vertexArrayObject);
	glCreateBuffers(1, &vertexBufferObject[POSITION]);

	glNamedBufferStorage(vertexBufferObject[POSITION], sizeof(cube), cube, 0);
	glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBufferObject[POSITION], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vertexArrayObject, 0, 4, GL_FLOAT, GL_FALSE, 0); //?
	glVertexArrayAttribBinding(vertexArrayObject, 0, 0);
}