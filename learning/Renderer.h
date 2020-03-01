#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "HeightMap.h"
#include "Camera.h"

#include "ParticleSystem/ParticleMaster.h"

class Renderer : public RenderBase {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void Update(float dt) override;
	virtual void Render() override;

	void renderObject();
	void renderSkyBox();

	float startingTime;
	float framesPerSecond;
	float oneSecond;

protected:
	
	RenderObject*		object;
	RenderObject*		skybox;

	Camera*				camera;

	Matrix4				projMatrix;
	Matrix4				modelMatrix;

	ParticleMaster*		particleMaster;

	void FPSCalculation(float dt);
	void CreateParticle();
};


