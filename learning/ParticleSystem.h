#pragma once

#include "glew.h"
#include "../basic/math/Vector3.h"

class ParticleSystem
{
	struct Particle {
		Vector3 pos, speed;
		unsigned char r, g, b, a; // Color
		float size, angle, weight;
		float life; // Remaining life of the particle. if < 0 : dead and unused.

	};

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

	Particle ParticlesContainer[MaxParticles];

	int LastUsedParticle = 0;

	int FindUnusedParticle() {
		for (int i = LastUsedParticle; i < MaxParticles; i++) {
			if (ParticlesContainer[i].life < 0) {
				LastUsedParticle = i;
				return i;
			}
		}

		for (int i = 0; i < LastUsedParticle; i++) {
			if (ParticlesContainer[i].life < 0) {
				LastUsedParticle = i;
				return i;
			}
		}

		return 0; // All particles are taken, override the first one
	}
};

