#pragma once

#include "Particle.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "../basic/math/Matrix4.h"

#include <list>

using namespace std;

class ParticleSystemBase
{
public:
	enum ParticleSystemType {
		DEFAULT,
		DROPLET,
		SPLASH,
		MAX_NUMBER
	};

	enum ParticleBuffer {
		BASEMESH_BUFFER,
		MAX_BUFFER
	};


	ParticleSystemBase(Camera* camera, Matrix4 projMatrix,
		unsigned int number = 50, Vector3 position = { 0,0,-50 }, float life = 4.f, unsigned int variation = 0, int initialForce = 50);
	virtual ~ParticleSystemBase();

	virtual void Update(float dt);
	virtual void Render();

	void SetShape(const float shape[16]);

protected:
	Texture* texture;
	float shapeArray[16];
	GLuint vao;
	GLuint vbo[MAX_BUFFER];
	ParticleSystemType systemType;

	Shader* particleShader;

	list<Particle> particleList;
	void UpdateMatrix(Particle& p, const Matrix4& viewMatrix);

private:
	Camera* camera;
	Matrix4 projMatrix;

	unsigned int number;
	Vector3 position;
	float life;
	unsigned int variation;
	int initialForce;
};

