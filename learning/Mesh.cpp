#include "Mesh.h"

Mesh::Mesh() :
	renderType(GL_TRIANGLES),
	numOfVertices(0),
	numOfIndex(0),
	vao(0)
{
	for (int i = 0; i < MAXBUFFER; ++i) {
		vbo[i] = 0;
	}
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(MAXBUFFER, vbo);
}

void Mesh::CreateTriangle() {
	numOfVertices = 4;
	numOfIndex = 6;
	renderType = GL_TRIANGLES;

	position.push_back(Vector3(-0.5f, -0.5f, 10.0f));
	position.push_back(Vector3(-0.5f, 0.5f, 0.0f));
	position.push_back(Vector3(0.5f, -0.5f, 10.0f));
	position.push_back(Vector3(0.5f, 0.5f, 0.0f));

	color.push_back(Vector3(1.0f, 0.0f, 0.0f));
	color.push_back(Vector3(0.0f, 1.0f, 0.0f));
	color.push_back(Vector3(0.0f, 0.0f, 1.0f));
	color.push_back(Vector3(0.0f, 1.0f, 1.0f));

	texCoord.push_back(Vector2(0.0f, 1.0f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	texCoord.push_back(Vector2(1.0f, 0.0f));

	index.push_back(0);
	index.push_back(1);
	index.push_back(2);

	index.push_back(1);
	index.push_back(2);
	index.push_back(3);

	BufferDataToGPU();
}

void Mesh::CreateQuad()
{
	numOfVertices = 4;
	renderType = GL_TRIANGLE_STRIP;

	position.push_back(Vector3(-1.0f, -1.0f, 0.0f));
	position.push_back(Vector3(-1.0f, 1.0f, 0.0f));
	position.push_back(Vector3(1.0f, -1.0f, 0.0f));
	position.push_back(Vector3(1.0f, 1.0f, 0.0f));

	BufferDataToGPU();
}

void Mesh::Draw()
{
	EnableAttribs();
	if (vbo[INDEX]) {
		glDrawElements(renderType, numOfIndex, GL_UNSIGNED_INT, (void*)(index.data()));
	}
	else {
		glDrawArrays(renderType, 0, numOfVertices);
	}
	DisableAttribs();
}

void Mesh::BufferDataToGPU()
{
	glCreateVertexArrays(1, &vao);
	if (!position.empty()) {
		glCreateBuffers(1, &vbo[POSITION]);
		glNamedBufferStorage(vbo[POSITION], numOfVertices * sizeof(Vector3), (void*)(position.data()), 0);
		glVertexArrayVertexBuffer(vao, POSITION, vbo[POSITION], 0, sizeof(Vector3));
		glVertexArrayAttribFormat(vao, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, POSITION, POSITION);
	}
	if (!color.empty()) {
		glCreateBuffers(1, &vbo[COLOR]);
		glNamedBufferStorage(vbo[COLOR], numOfVertices * sizeof(Vector3), (void*)(color.data()), 0);
		glVertexArrayVertexBuffer(vao, COLOR, vbo[COLOR], 0, sizeof(Vector3));
		glVertexArrayAttribFormat(vao, COLOR, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, COLOR, COLOR);
	}
	if (!texCoord.empty()) {
		glCreateBuffers(1, &vbo[TEXTURE]);
		glNamedBufferStorage(vbo[TEXTURE], numOfVertices * sizeof(Vector2), (void*)(texCoord.data()), 0);
		glVertexArrayVertexBuffer(vao, TEXTURE, vbo[TEXTURE], 0, sizeof(Vector2));
		glVertexArrayAttribFormat(vao, TEXTURE, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, TEXTURE, TEXTURE);
	}
	if (!index.empty()) {
		glCreateBuffers(1, &vbo[INDEX]);
		//lNamedBufferStorage(vbo[INDEX], numOfIndex * sizeof(GLuint), (void*)(index.data()), 0);
	}
}

void Mesh::EnableAttribs()
{
	glBindVertexArray(vao);
	if (!position.empty()) {
		glEnableVertexAttribArray(POSITION);
	}
	if (!color.empty()) {
		glEnableVertexAttribArray(COLOR);
	}
	if (!texCoord.empty()) {
		glEnableVertexAttribArray(TEXTURE);
	}
}

void Mesh::DisableAttribs()
{
	if (!position.empty()) {
		glDisableVertexAttribArray(POSITION);
	}
	if (!color.empty()) {
		glDisableVertexAttribArray(COLOR);
	}
	if (!texCoord.empty()) {
		glDisableVertexAttribArray(TEXTURE);
	}
	glBindVertexArray(0);
}
