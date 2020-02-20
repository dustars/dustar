#pragma once

#include "glew.h"
#include <list>
#include "../basic/math/Vector3.h"

constexpr auto GRAVITY = -10.f;

class Particle
{
public:
	Particle();
	Particle(Vector3 position, Vector3 velocity, float life, float rotation, float scale, float weight);
	~Particle();
	// Update a single particle
	bool Update(float dt);

	Vector3 position;
	Vector3 velocity;

	float rotation;
	float scale;
	float weight;
	float life; // Remaining life of the particle. if < 0 : dead and unused.

	float elapsedTime;
};
