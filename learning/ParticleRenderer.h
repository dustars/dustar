#pragma once

#include "Particle.h"
#include "Camera.h"
#include "Shader.h"

class ParticleRenderer
{

public:
	enum ParticleBuffer {
		BASEMESH_BUFFER,
		CENTER_BUFFER,
		COLOR_BUFFER,
		MAX_BUFFER
	};

	ParticleRenderer(Matrix4 projMatrix, Camera* camera = nullptr);
	~ParticleRenderer();

	static void ParticleUpdate(float dt) {
		Particle::UpdateParticleList(dt);
	}

	inline const GLuint&	GetVBO() { return *vbo; }
	inline const Shader*	GetShader() { return particleShader; }

	GLuint vao;
	GLuint vbo[MAX_BUFFER];

	void RenderParticle();
	void UpdateMatrix(Particle& p, const Matrix4& viewMatrix);

protected:
	Shader*		particleShader;
	Camera*		camera;
	Matrix4		projMatrix;
};

