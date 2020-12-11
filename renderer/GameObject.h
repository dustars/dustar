/*
	TODO:
	1. Maybe a string member variable is necessary for the ID purpose.
	2. texture could be more than one.
	3. Transform should have a local type instead of a pointer. (pointer in class is dangerous anyway)
*/

#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class GameObject
{
public:
	GameObject() : mesh(new Mesh), texture(new Texture) {}
	~GameObject() {}

	GLuint		GetProgram()	{ return shader->GetProgram(); }
	Mesh*		GetMesh()		{ return mesh; }
	Texture*	GetTexture()	{ return texture; }

	void SetMesh(Mesh* m) { delete mesh; mesh = m; }
	bool SetShader(string vs, string fs, string gs = "", string cs = "", string es = "") {
		shader = new Shader(vs, fs, gs, cs, es);
		return shader->LinkProgram();
	}

	void Draw() { mesh->Draw(); }

protected:
	Shader*		shader	= nullptr;
	Mesh*		mesh	= nullptr;
	Texture*	texture = nullptr;
};

