#include "RenderObject.h"

RenderObject::RenderObject()
{
	shader		= new Shader();
	mesh		= new Mesh();
	transform	= new Transform();
}

RenderObject::~RenderObject()
{
	delete shader;
	delete mesh;
	delete transform;
}