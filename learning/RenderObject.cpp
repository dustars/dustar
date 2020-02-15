#include "RenderObject.h"

RenderObject::RenderObject()
{
	shader		= nullptr;
	mesh		= new Mesh();
	transform	= new Transform();
}

RenderObject::~RenderObject()
{
	delete shader;
	delete mesh;
	delete transform;
}

bool RenderObject::SetShader(string vs, string fs, string gs, string cs, string es)
{
	shader = new Shader(vs, fs, gs, cs, es);
	return shader->LinkProgram();
}
