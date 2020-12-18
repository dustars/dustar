/*
	TODO:
	1. Maybe a string member variable is necessary for the ID purpose.
	2. texture could be more than one.
	3. Transform should have a local type instead of a pointer. (pointer in class is dangerous anyway)
*/

#pragma once
#include <memory>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class GameObject
{
public:
	GameObject() {}

	GLuint GetProgram()	{ return shader->GetProgram(); }
	GLuint GetTexture()	{ return texture->GetTexture(); }

	void SetMesh(Mesh* m) { mesh.reset(m); }
	bool SetShader(string vs, string fs, string gs = "", string cs = "", string es = "") {
		shader.reset(new Shader(vs, fs, gs, cs, es));
		return shader->LinkProgram();
	}
	bool SetTexture(const string& path) { texture.reset(new Texture); return texture->SetTexture(path); }
	void SetTexture(const char* right, const char* left, const char* top, const char* bottom, const char* back, const char* front)
	{ texture.reset(new Texture); texture->SetTexture(right, left, top, bottom, back, front); }

	void Draw() { mesh->Draw(); }

	//Quick operations
	void SetMeshAsQuad() { mesh.reset(new Mesh); mesh->CreatePlane(); }

protected:
	std::unique_ptr<Shader>		shader;
	std::unique_ptr<Mesh>		mesh;
	std::unique_ptr<Texture>	texture;
};

