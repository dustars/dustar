#include "Particle.h"

std::list<Particle> Particle::ParticleList;

Particle::Particle() : position({ 0,0,0 }), velocity({ 0,0,0 }), color({1.0,1.0,1.0}), rotation(0), scale(0), weight(0), life(0), elapsedTime(0)
{
}

Particle::Particle(Vector3 position, Vector3 velocity, Vector3 color, float rotation, float scale, float weight, float life) :
	position(position), velocity(velocity), color(color), rotation(rotation), scale(scale), weight(weight), life(life), elapsedTime(0)
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
