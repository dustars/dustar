#pragma once

#include "Particle.h"
#include "Camera.h"

class ParticleRenderer
{

public:
	enum ParticleBuffer {
		BASEMESH_BUFFER,
		CENTER_BUFFER,
		COLOR_BUFFER,
		MAX_BUFFER
	};

	ParticleRenderer(){}
	ParticleRenderer(Matrix4 projMatrix, Camera* camera = nullptr);
	~ParticleRenderer();

	static const int MaxParticles = 100000;

	static void ParticleUpdate(float dt) {
		Particle::UpdateParticleList(dt);
	}

	inline const GLuint& GetVBO() { return *vbo; }
	inline const GLuint GetProgram() { return program; }

	GLuint vao;
	GLuint vbo[MAX_BUFFER];

	void RenderParticle();
	void UpdateMatrix(Particle& p, const Matrix4& viewMatrix);

protected:
	GLuint		program;
	Camera*		camera;

	void createShader();
};

