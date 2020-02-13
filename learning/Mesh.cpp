#include "Mesh.h"

Mesh::Mesh()
{
	vao = 0;

	for (int i = 0; i < MAXBUFFER; ++i) {
		vbo[i] = 0;
	}

	numOfVertices = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(MAXBUFFER, vbo);
}

void Mesh::CreateTriangle() {
	numOfVertices = 3;

	glCreateVertexArrays(1, &vao);
	//glGenVertexArrays(1, &vertexArrayObject);  // Note the difference between Gen and Create
	//glBindVertexArray(vertexArrayObject); //Why I need this?

	glCreateBuffers(MAXBUFFER, vbo);
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
	glNamedBufferStorage(vbo[POSITION], sizeof(position), position, 0);
	glNamedBufferStorage(vbo[COLOR], sizeof(color), color, 0);
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

	glVertexArrayVertexBuffer(vao, 0, vbo[POSITION], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);

	glVertexArrayVertexBuffer(vao, 1, vbo[COLOR], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 1, 1);

	//glVertexArrayAttribBinding(vertexArrayObject, positionIndex, 0);
	//glVertexArrayAttribBinding(vertexArrayObject, colorIndex, 0);

	//glVertexArrayAttribFormat(vertexArrayObject, positionIndex, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
	//glVertexArrayAttribFormat(vertexArrayObject, colorIndex, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, r));

	//glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBufferObject[TEXTURE], 0, sizeof(vertex));
}

void Mesh::CreateCube() {
	numOfVertices = 36;

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo[POSITION]);

	glNamedBufferStorage(vbo[POSITION], sizeof(cube), cube, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo[POSITION], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0); //?
	glVertexArrayAttribBinding(vao, 0, 0);
}