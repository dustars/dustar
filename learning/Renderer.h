/*
	Some issues:
	1. Now the Tranform component from class RenderObject has no usages in this renderer.
	   I need to put it into use after having a thorough understanding of Transformation.
	2. Be careful about your resouce management, pointers in class are dangerous (using unique_ptr or something)

	Lightings:
	1. I've implemented the shading model from RTR 4rd, 5.3, it's called extended Gooch shading model.
	   But Why the color is so weird? Especially the hightlight area, something wrong with the normal?
	   I need to try the shading equation in Rich's model to see it's the problem of normal.
*/


#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "HeightMap.h"
#include "Trajectory.h"
#include "Camera.h"

#include "Lightings/PointLight.h"

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

	//Lightings
	PointLight*			pointLight1		= nullptr;

	ParticleMaster*		particleMaster	= nullptr;

	//Temp creations
	void CreateSkybox();
	void CreateTrajectory();

	//Utility
	const float renderFrames = 1000.f / 60.f;
	float oneFrame = 0;
};


