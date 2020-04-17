#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "HeightMap.h"
#include "Trajectory.h"
#include "Camera.h"

#include "ParticleSystem/ParticleMaster.h"

class ParticleMaster;

class Renderer : public RenderBase {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void Update(float dt) override;
	virtual void Render() override;

	void renderObject();
	void renderSkyBox();

	Camera* GetCamera() { return camera; }
	Matrix4& GetProjMatrix() { return projMatrix; }

protected:
	
	RenderObject*		object			= nullptr;
	RenderObject*		trajectory		= nullptr;
	RenderObject*		skybox			= nullptr;

	Camera*		camera					= nullptr;
	Matrix4		projMatrix;
	Matrix4		modelMatrix;

	ParticleMaster*		particleMaster	= nullptr;

	//Temp creations
	void CreateSkybox();
	void CreateTrajectory();

	//Utility
	const float renderFrames = 1000.f / 60.f;
	float oneFrame = 0;
};


