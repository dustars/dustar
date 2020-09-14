#pragma once

#include <GL/glew.h>
#include <list>
#include "math/math.h"

constexpr auto GRAVITY = -10.f;

class Particle
{
	friend class ParticleSystem;
public:
	Particle(Vector3 position = Vector3(0,0,0), Vector3 velocity = Vector3(0, 0, 0),
			 float life = 0.f, float rotation = 0.f, float scale = 0.f, float weight = 0.f);
	// Update a single particle
	bool Update(float dt);

	void SetTextureOffset(int numRows, int index1, int index2);

	Vector2 GetTexOffset1() { return texOffset1; }
	Vector2 GetTexOffset2() { return texOffset2; }
	float  GetBlendFactor() { return blendFactor;}

private:
	Vector3 position;
	Vector3 velocity;

	float rotation;
	float scale;
	float weight;
	
	float life; // Remaining life of the particle. if < 0 : dead and unused.
	float elapsedTime;
	float distanceFromCamera;  // For correct rendering

	// Texture Atlas
	Vector2 texOffset1;
	Vector2 texOffset2;
	float blendFactor;
};
