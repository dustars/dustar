/*
	Description:
	The current renderer class is literally where everything happens.
	It manages resources, is reponsible for updating, maintains all
	unitity methods, has debugger, has temporary testing code, and of
	course the rendering!
	In a word, it's a mess.
	It needs to be refactored in the near future.
	6/15/2020

	1. Get rid of the voxeliazation part (decouple it ASAP!)
	2. Create a manager class or at least a vector to contain all those RenderObjects.
*/


#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "Camera.h"
#include "Debug.h"
#include "GeneralFrameBuffer.h"

#include "HeightMap.h"
#include "Trajectory.h"

#include "Lightings/PointLight.h"
#include "Cloud.h"

#include "ParticleSystem/ParticleMaster.h"

#include <memory>

#define TESTING
constexpr auto MAPWIDTH = 500;
constexpr auto MAPLENGTH = 500;

class ParticleMaster;

class Renderer : public RenderBase
{
public:
	Renderer(Window& parent); //Currently also used as an initialization method for everything.
	~Renderer() override;

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
	Debug debugger;

private:
	//The FBO contains the final image of the object-based rendering stage.
	std::unique_ptr<FrameBuffer> renderFBO;

	//Testing
	void Testing();
	void TestRendering();

	//For voxelization
	//Get rid of them, make them a new class pleaseeeeeeeeeeeeeeeeee, What a mess
	GLuint depthTex[6];
	GLuint frameBuffer[6];

	RenderObject voxelShader;
	GLuint voxel3DTexture;

	void CreateVoxelizationResources();
	void RealTimeVoxelization(); //I know this is a unacceptably stupid idea....

	//For Cloud
	RenderObject cloudShader;
	GLuint worleyTex;
	//Weather Map

	void CreateCloud();
	void RenderCloud();
	void CreateCloud3DTexture();
	void CreatePerlinWorleyNoise();

	//Unitiies
	void SaveAsImage(int w, int h, const void* data, int channels = 3, int quality = 100);

	float Remap(float value, float oldMin, float oldMax, float newMin, float newMax) {
		return newMin + (((value - oldMin) / (oldMax - oldMin)) * (newMax - newMin));
	}

	float Clamp(float value, float min, float max) {
		if (value > max) return max;
		if (value < min) return min;
		return value;
	}

public:
	//Save the initial frame in "demo/screenshot.jpg"
	void ScreenShot(); //Shouldn't this method be in the camera class?
	void Voxelization(int);
};


