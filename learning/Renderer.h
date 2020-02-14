#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "Camera.h"

#include "ParticleSystem.h"

class Renderer : public RenderBase {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void Update(float dt) override;
	virtual void Render() override;

	void renderObject();
	void renderParticle();

	float startingTime;
	float framesPerSecond;
	float oneSecond;

protected:
	
	RenderObject*	object;
	Camera*			camera;

	Particle*		particle;

	void FPSCalculation(float dt);
};


