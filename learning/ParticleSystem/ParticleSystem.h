#pragma once

#include "Particle.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../Shader.h"
#include "../Camera.h"
#include <list>

using namespace std;

class Renderer;

class ParticleSystem
{
	friend class ParticleMaster;
public:
	static Renderer* renderer;

	enum ParticleBuffer {
		BASEMESH_BUFFER,
		TEXTURE_BUFFER,
		MAX_BUFFER
	};

	ParticleSystem(string texFile, int numOfRows = 1,
		unsigned int number = 50, Vector3 position = { 0,0,-50 }, float life = 4.f,
		unsigned int variation = 0, int initialForce = 50);
	virtual ~ParticleSystem();

	virtual void Update(float);
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
	void UpdateTextureCoordinate(Particle& p);

	Shader* particleShader;

	unsigned int number;
	Vector3 position;
	float life;
	unsigned int variation;
	int initialForce;
};

