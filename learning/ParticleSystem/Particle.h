#pragma once

#include "glew.h"
#include <list>
#include "../basic/math/Vector3.h"
#include "../basic/math/Vector2.h"

constexpr auto GRAVITY = -10.f;

class Particle
{
public:

	friend class ParticleSystemBase;

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
	float distanceFromCamera;  // For correct rendering

	void SetTextureOffset(int numRows, int index1, int index2);

private:
	// Texture Atlas
	Vector2 texOffset1;
	Vector2 texOffset2;
	float blendFactor;
};
