#pragma once
#include "ParticleSystemBase.h"
#include "Camera.h"


class Rain : public ParticleSystemBase
{

public:
	Rain();

	void Update();

	void UpdateMatrix(Particle& p, const Matrix4& viewMatrix);

private:

	Camera*		camera;
	Matrix4		projMatrix;
};

