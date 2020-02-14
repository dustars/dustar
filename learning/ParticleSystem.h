#pragma once

#include "Particle.h"

class ParticleSystem
{


public:
	enum ParticleBuffer {
		BASEMESH_BUFFER,
		CENTER_BUFFER,
		COLOR_BUFFER,
		MAX_BUFFER
	};

	ParticleSystem();
	~ParticleSystem();

	inline GLuint& GetVBO() { return *vbo; }

	void ParticleUpdate(float dt);

	GLuint vao;
	GLuint vbo[MAX_BUFFER];

	static const int MaxParticles = 100000;

};

