#include "RenderObject.h"

RenderObject::RenderObject()
{
	shader		= nullptr;
	mesh		= new Mesh();
	texture		= new Texture();
	transform	= new Transform();
}

RenderObject::~RenderObject()
{
	delete shader;
	delete mesh;
	delete texture;
	delete transform;
}

void RenderObject::SetMesh(Mesh* m)
{
	if (!mesh) {
		delete mesh;
	}
	mesh = m;
}

bool RenderObject::SetShader(string vs, string fs, string gs, string cs, string es)
{
	shader = new Shader(vs, fs, gs, cs, es);
	return shader->LinkProgram();
}

void RenderObject::Draw()
{
	glBindTextureUnit(0, GetTexture()->GetTexture());
	GetMesh()->Draw();
}
