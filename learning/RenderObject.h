#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"

class RenderObject
{
public:
	RenderObject();
	~RenderObject();

	inline Shader*		GetShader()		{ return shader; }
	inline Mesh*		GetMesh()		{ return mesh; }
	inline Transform*	GetTransform()	{ return transform; }

protected:
	Shader*			shader;
	Mesh*			mesh;
	Transform*		transform;
};

