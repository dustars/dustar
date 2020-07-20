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
	3. Use unique_ptr for all pointer type data members?
	4. Debugger.
*/

#pragma once
#include "RenderBase.h"
#include "RenderObject.h"
#include "Camera.h"
#include "Debug.h"
#include "GeneralFrameBuffer.h"

#include "HeightMap.h"
#include "Trajectory.h"
#include "WorleyNoise.h"
#include "atmosphere/model.h"

#include "Lightings/PointLight.h"
#include "Cloud.h"

#include "ParticleSystem/ParticleMaster.h"

#include <memory>
#include <thread>

#define TESTING
//#define OFFLINE
#define THREADING
//#define RENDER_CLOUD
#define ATMOSPHERE
//#define TESTING_OBJECT

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
	//To render specific objects that are still under development/testing.
	void TestRendering();

	Camera* GetCamera()		const{ return camera; }
	Matrix4 GetProjMatrix() const{ return projMatrix; }

private:
	//Render Objects
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

	//The FBO contains the result of Rasterization rendering.
	std::unique_ptr<FrameBuffer> renderFBO;

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
	GLuint highFreqNoiseTex;
	GLuint lowFreqNoiseTex;
	GLuint weatherMapTex;

	void CreateCloud();
	void RenderCloud();
	void CreateCloud3DTexture();

	//For atmospheic scattering
	std::unique_ptr<atmosphere::Model> atmosphereScattering;
	RenderObject atmosphereScatteringShader;
	//I port the demo by https://ebruneton.github.io/precomputed_atmospheric_scattering/
	//into these two methods (model initialization and rendering)
	void CreateAtmosphericScatteringModel();
	void RenderAtmosphericScatteringModel();

public:
	//Save the initial frame in "demo/screenshot.jpg"
	void ScreenShot(std::string filename = "screenshot"); //Shouldn't this method be in the camera class?
	void Voxelization(int);
};


