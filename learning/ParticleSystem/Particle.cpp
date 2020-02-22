#include "Particle.h"

Particle::Particle() : position({ 0,0,0 }), velocity({ 0,0,0 }), life(0), rotation(0), scale(0), weight(0), elapsedTime(0), distanceFromCamera(0)
{
}

Particle::Particle(Vector3 position, Vector3 velocity, float life, float rotation, float scale, float weight) :
	position(position), velocity(velocity), life(life), rotation(rotation), scale(scale), weight(weight), elapsedTime(0), distanceFromCamera(0)
{
}

Particle::~Particle()
{
}

bool Particle::Update(float dt)
{
	dt *= 0.001f;
	velocity.y += (GRAVITY * weight * dt);
	Vector3 change(velocity);
	position += change * dt;
	elapsedTime += dt;
	return elapsedTime < life;
}
