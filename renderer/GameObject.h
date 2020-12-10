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
#include <memory>

class GameObject
{
public:
	GameObject() : mesh(new Mesh), texture(new Texture) {}
	~GameObject() {}

	GLuint		GetProgram()	{ return shader.get()->GetProgram(); }
	Mesh*		GetMesh()		{ return mesh.get(); }
	Texture*	GetTexture()	{ return texture.get(); }

	void SetMesh(Mesh* m) { mesh.reset(m); }
	bool SetShader(string vs, string fs, string gs = "", string cs = "", string es = "") {
		shader.reset(new Shader(vs, fs, gs, cs, es));
		return shader->LinkProgram();
	}

	void Draw() { GetMesh()->Draw(); }

protected:
	unique_ptr<Shader>		shader;
	unique_ptr<Mesh>		mesh;
	unique_ptr<Texture>		texture;
};

