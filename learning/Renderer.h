#pragma once
#include "RenderBase.h"
#include "Shader.h"
#include "Mesh.h"

class Renderer : public RenderBase {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);
	virtual void RenderScene() override;

	void CreateTriangle();
	void CreateCube();

	float startingTime;

protected:
	
	Shader* program;

	GLuint vertexArrayObject;
	GLuint vertexBufferObject[MAXBUFFER];

	unsigned short int numVertices = 3;
};


