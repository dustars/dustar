#include "Particle.h"

Particle::Particle() :
	position({ 0,0,0 }),
	velocity({ 0,0,0 }),
	life(0),
	rotation(0),
	scale(0),
	weight(0),
	elapsedTime(0),
	distanceFromCamera(0),
	blendFactor(0)
{
}

Particle::Particle(Vector3 position, Vector3 velocity, float life, float rotation, float scale, float weight) :
	position(position),
	velocity(velocity),
	life(life),
	rotation(rotation),
	scale(scale),
	weight(weight),
	elapsedTime(0),
	distanceFromCamera(0),
	blendFactor(0)
{
}

Particle::~Particle()
{
}

bool Particle::Update(float dt)
{
	dt *= 0.001f;
	velocity.y += (GRAVITY * weight * dt);
	position += velocity * dt;
	elapsedTime += dt;
	return elapsedTime < life;
}

void Particle::SetTextureOffset(int numRows, int index1, int index2)
{
	int column1 = index1 % numRows;
	int row1 = index1 / numRows;

	texOffset1.x = (float)column1 / numRows;
	texOffset1.y = (float)row1 / numRows;

	int column2 = index2 % numRows;
	int row2 = index2 / numRows;

	texOffset2.x = (float)column2 / numRows;
	texOffset2.y = (float)row2 / numRows;
}
