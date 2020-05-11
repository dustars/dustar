/*
	Some issues:
	1. Pointer in class
*/


#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "Camera.h"

#include "HeightMap.h"
#include "Trajectory.h"

#include "Lightings/PointLight.h"

#include "ParticleSystem/ParticleMaster.h"

class ParticleMaster;

class Renderer : public RenderBase
{
public:
	Renderer(Window& parent);
	~Renderer(void) override;

	void Update(float dt) override;
	void Render() override;

	Camera* GetCamera()		const{ return camera; }
	Matrix4 GetProjMatrix() const{ return projMatrix; }

protected:
	
	RenderObject*		object			= nullptr;
	RenderObject*		trajectory		= nullptr;
	RenderObject*		skybox			= nullptr;

	Camera*		camera					= nullptr;
	Matrix4		projMatrix;
	Matrix4		modelMatrix;

	//Lightings
	PointLight*			pointLight1		= nullptr;

	ParticleMaster*		particleMaster	= nullptr;

	//Temp creations
	void CreateSkybox();
	void CreateTrajectory();

	//Rendering
	void renderObject();
	void renderSkyBox();

	//Utility
	const float renderFrames = 1000.f / 60.f;
	float oneFrame = 0;

private:
	void testing();
};


