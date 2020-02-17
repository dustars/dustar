#pragma once

#include "glew.h"
#include <list>
#include "../basic/math/Vector3.h"

constexpr auto GRAVITY = -10.f;

class Particle
{
	friend class ParticleRenderer;

public:
	// Contain all particles
	static std::list<Particle> ParticleList;

	// Iterate and update all particles in the listf
	static void UpdateParticleList(float dt) {
		for (auto i = ParticleList.begin(); i != ParticleList.end(); ++i) {
			bool ifAlived = i->Update(dt);
			if (!ifAlived) {
				i = ParticleList.erase(i);
				if (i == ParticleList.end()) {
					break;
				}
			}
		}
	}

	inline static void AddParticle(Vector3 position, Vector3 velocity, Vector3 color, float rotation, float scale, float weight, float life) {
		Particle newP(position, velocity, color, rotation, scale, weight, life);
		ParticleList.push_back(newP);
	}

	static void Fountain(int number = 50, Vector3 position = { 0,0,-50 }, Vector3 color = { 1.0f,1.0f,1.0f },
						float life = 4.f, unsigned int variation = 0, int initialForce = 50) {
		for (int i = 0; i < number; ++i) {
			// random velocity of unit circle
			float dirX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2)) - 1;
			float dirZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2)) - 1;
			Vector3 velocity(dirX, 0, dirZ);
			velocity.Normalise();
			
			if (!variation) {
				float randY = rand() % (variation + 1);
			}
			velocity.x *= initialForce;
			velocity.y = variation * initialForce;
			velocity.z *= initialForce;
			Particle newP(position, velocity, color, 0, 1, 1, life);
			ParticleList.push_back(newP);
		}
	}


	Particle();
	Particle(Vector3 position, Vector3 velocity, Vector3 color, float rotation, float scale, float weight, float life);
	~Particle();
	// Update a single particle
	bool Update(float dt);

	inline Vector3 const	GetPosition()	{ return position; }
	inline Vector3 const	GetColor()		{ return color; }
	inline float const		GetRotation()	{ return rotation; }
	inline float const		GetScale()		{ return scale; }

private:
	Vector3 position;
	Vector3 velocity;
	Vector3 color;

	float rotation;
	float scale;
	float weight;
	float life; // Remaining life of the particle. if < 0 : dead and unused.

	float elapsedTime;
};
