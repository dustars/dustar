/*
	TODO:
	1. Maybe a string member variable is necessary for the ID purpose.
	2. texture could be more than one.
	3. Transform should have a local type instead of a pointer.
*/


#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

class RenderObject
{
public:
	RenderObject();
	~RenderObject();

	inline Shader*		GetShader()		{ return shader; }
	inline Mesh*		GetMesh()		{ return mesh; }
	inline Texture*		GetTexture()	{ return texture; }
	inline Transform*	GetTransform()	{ return transform; }

	void SetMesh(Mesh* m);

	bool SetShader(string vs, string fs, string gs = "", string cs = "", string es = "");

	void Draw();

protected:
	Shader*			shader;
	Mesh*			mesh;
	Texture*		texture;
	Transform*		transform;
};

