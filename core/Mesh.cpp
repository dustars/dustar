#include "Mesh.h"

Mesh::Mesh() :
	renderType(GL_TRIANGLES),
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

void Mesh::CreatePlane()
{
	renderType = GL_TRIANGLES;

	position.push_back(Vector3(-1.0f, 1.0f, 0.0f));
	position.push_back(Vector3(-1.0f, -1.0f, 0.0f));
	position.push_back(Vector3(1.0f, 1.0f, 0.0f));
	position.push_back(Vector3(1.0f, -1.0f, 0.0f));

	color.push_back(Vector3(1.0f, 0.0f, 0.0f));
	color.push_back(Vector3(0.0f, 1.0f, 0.0f));
	color.push_back(Vector3(0.0f, 0.0f, 1.0f));
	color.push_back(Vector3(1.0f, 0.0f, 1.0f));
	
	texCoord.push_back(Vector2(0.0f, 0.0f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	texCoord.push_back(Vector2(1.0f, 1.0f));

	Vector3 tempNormal = Vector3::Cross(position[1] - position[0], position[2] - position[0]);
	normal.push_back(tempNormal);
	normal.push_back(tempNormal);
	normal.push_back(tempNormal);
	normal.push_back(tempNormal);

	index.push_back(0);
	index.push_back(1);
	index.push_back(2);
	index.push_back(2);
	index.push_back(1);
	index.push_back(3);

	BufferDataToGPU();
}

void Mesh::CreateCube()
{
	renderType = GL_TRIANGLES;

	position.push_back(Vector3(-0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	position.push_back(Vector3(0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(-0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(-0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));

	position.push_back(Vector3(-0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	position.push_back(Vector3(0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(-0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(-0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));

	position.push_back(Vector3(-0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(-0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(-0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(-0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(-0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	position.push_back(Vector3(-0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));

	position.push_back(Vector3(0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	position.push_back(Vector3(0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));

	position.push_back(Vector3(-0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(-0.5f, -0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	position.push_back(Vector3(-0.5f, -0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));

	position.push_back(Vector3(-0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));
	position.push_back(Vector3(0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(1.0f, 1.0f));
	position.push_back(Vector3(0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(1.0f, 0.0f));
	position.push_back(Vector3(-0.5f, 0.5f, 0.5f));
	texCoord.push_back(Vector2(0.0f, 0.0f));
	position.push_back(Vector3(-0.5f, 0.5f, -0.5f));
	texCoord.push_back(Vector2(0.0f, 1.0f));

	GenerateNormals();
	BufferDataToGPU();
}

void Mesh::CreateQuad()
{
	renderType = GL_TRIANGLE_STRIP;

	position.push_back(Vector3(-1.0f, 1.0f, 0.0f));
	position.push_back(Vector3(-1.0f, -1.0f, 0.0f));
	position.push_back(Vector3(1.0f, 1.0f, 0.0f));
	position.push_back(Vector3(1.0f, -1.0f, 0.0f));

	BufferDataToGPU();
}

void Mesh::Draw()
{
	glBindVertexArray(vao);
	EnableAttribs();
	if (vbo[INDEX]) {
		glDrawElements(renderType, index.size(), GL_UNSIGNED_INT, static_cast<void*>(index.data()));
	}
	else {
		glDrawArrays(renderType, 0, position.size());
	}
	DisableAttribs();
	glBindVertexArray(0);
}

void Mesh::Update(float dt)
{
	UpdateDataToGPU();
}

void Mesh::BufferDataToGPU()
{
	glCreateVertexArrays(1, &vao);
	if (!position.empty()) {
		glCreateBuffers(1, &vbo[POSITION]);
		glNamedBufferStorage(vbo[POSITION], position.size() * sizeof(Vector3), static_cast<void*>(position.data()), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(vao, POSITION, vbo[POSITION], 0, sizeof(Vector3));
		glVertexArrayAttribFormat(vao, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, POSITION, POSITION);
	}
	if (!color.empty()) {
		glCreateBuffers(1, &vbo[COLOR]);
		glNamedBufferStorage(vbo[COLOR], color.size() * sizeof(Vector3), static_cast<void*>(color.data()), 0);
		glVertexArrayVertexBuffer(vao, COLOR, vbo[COLOR], 0, sizeof(Vector3));
		glVertexArrayAttribFormat(vao, COLOR, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, COLOR, COLOR);
	}
	if (!texCoord.empty()) {
		glCreateBuffers(1, &vbo[TEXTURE]);
		glNamedBufferStorage(vbo[TEXTURE], texCoord.size() * sizeof(Vector2), static_cast<void*>(texCoord.data()), 0);
		glVertexArrayVertexBuffer(vao, TEXTURE, vbo[TEXTURE], 0, sizeof(Vector2));
		glVertexArrayAttribFormat(vao, TEXTURE, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, TEXTURE, TEXTURE);
	}
	if (!normal.empty()) {
		glCreateBuffers(1, &vbo[NORMAL]);
		glNamedBufferStorage(vbo[NORMAL], normal.size() * sizeof(Vector3), static_cast<void*>(normal.data()), 0);
		glVertexArrayVertexBuffer(vao, NORMAL, vbo[NORMAL], 0, sizeof(Vector3));
		glVertexArrayAttribFormat(vao, NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, NORMAL, NORMAL);
	}
	if (!index.empty()) {
		glCreateBuffers(1, &vbo[INDEX]);
		//The data is directly fed into GPU by calling glDrawElements()?
		//lNamedBufferStorage(vbo[INDEX], numOfIndex * sizeof(GLuint), (void*)(index.data())，0);
	}
}

void Mesh::UpdateDataToGPU()
{
	// For trajectory update (only updates position)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	if (!position.empty()) {
		glNamedBufferSubData(vbo[POSITION], 0, position.size() * sizeof(Vector3), static_cast<void*>(position.data()));
	}
}

void Mesh::EnableAttribs()
{
	if (!position.empty()) {
		glEnableVertexAttribArray(POSITION);
	}
	if (!color.empty()) {
		glEnableVertexAttribArray(COLOR);
	}
	if (!texCoord.empty()) {
		glEnableVertexAttribArray(TEXTURE);
	}
	if (!normal.empty()) {
		glEnableVertexAttribArray(NORMAL);
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
	if (!normal.empty()) {
		glDisableVertexAttribArray(NORMAL);
	}
}

// From NCLGL, rich's implementation
void Mesh::GenerateNormals() 
{
	for (GLuint i = 0; i < position.size(); ++i) {
		normal.push_back(Vector3());
	}
	if (!index.empty()) { // Generate per - vertex normals
		for (GLuint i = 0; i < index.size(); i += 3) {
			unsigned int a = index[i];
			unsigned int b = index[i + 1];
			unsigned int c = index[i + 2];

			Vector3 tempNormal = Vector3::Cross((position[b] - position[a]), (position[c] - position[a]));

			normal[a] += tempNormal;
			normal[b] += tempNormal;
			normal[c] += tempNormal;
		}
	}
	else { // It's just a list of triangles , so generate face normals
		for (GLuint i = 0; i < position.size(); i += 3) {
			Vector3& a = position[i];
			Vector3& b = position[i + 1];
			Vector3& c = position[i + 2];

			Vector3 tempNormal = Vector3::Cross(b - a, c - a);

			normal[i] = tempNormal;
			normal[i + 1] = tempNormal;
			normal[i + 2] = tempNormal;
		}
	}
	for (GLuint i = 0; i < position.size(); ++i) {
		normal[i].Normalise();
	}
}
