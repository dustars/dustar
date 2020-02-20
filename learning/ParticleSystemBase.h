#pragma once

#include "Particle.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

#include <list>

using namespace std;

class ParticleSystemBase
{
public:
	enum ParticleBuffer {
		BASEMESH_BUFFER,
		TEXTURE_BUFFER,
		MAX_BUFFER
	};

	ParticleSystemBase(Matrix4 projMatrix, string texFile, Camera* camera = nullptr, Shader* shader = nullptr,
		unsigned int number = 50, Vector3 position = { 0,0,-50 }, float life = 4.f,
		unsigned int variation = 0, int initialForce = 50);
	virtual ~ParticleSystemBase();

	virtual void Update(float dt);
	virtual void Render();

	void SetShape(const float shape[16]);

protected:
	Texture* texture;

	float shapeArray[16];
	GLuint vao;
	GLuint vbo[MAX_BUFFER];

	list<Particle> particleList;

	void EmitParticles();
	void UpdateMatrix(Particle& p, const Matrix4& viewMatrix);

	Shader* particleShader;

private:
	Camera* camera;
	Matrix4 projMatrix;

	unsigned int number;
	Vector3 position;
	float life;
	unsigned int variation;
	int initialForce;
};

