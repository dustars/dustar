#pragma once

#include "Particle.h"
#include "../Renderer.h"
#include "../Texture.h"
#include "../Shader.h"
#include "../Camera.h"
#include <list>

using namespace std;

class Renderer;

enum class EmitType {
	BASIC,
	TRAJECTORY
};

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

	ParticleSystem(unsigned = 50, Vector3 = { 0,0,-50 }, float = 4.f, unsigned = 0, int = 50, EmitType = EmitType::BASIC);
	~ParticleSystem();

	void Update(float);
	void Render();

	void SetShape(const float shape[16]);

protected:
	void EmitFountain();
	void EmitTrajectory();

	void UpdateMatrix(Particle& p, const Matrix4& viewMatrix);
	void UpdateTextureCoordinate(Particle& p);

	// Render part
	shared_ptr<Texture> texture;
	shared_ptr<Shader>	particleShader;

	float shapeArray[16];
	GLuint vao;
	GLuint vbo[MAX_BUFFER];

	// System part
	list<Particle> particleList; // total particles
	EmitType type;

	// General properties for each particle
	unsigned number;
	Vector3 position;
	float life;
	unsigned variation;
	int initialForce;

	float elaspedTime;
	float currentThreshhold;
	float emitInterval;
};
