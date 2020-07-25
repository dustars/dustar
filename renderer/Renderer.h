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
#include "Configuration.h"
#include "RenderBase.h"
#include "RenderObject.h"
#include "Camera.h"
#include "tools/Debug.h"
#include "GeneralFrameBuffer.h"

#include "HeightMap.h"
#include "Trajectory.h"
#include "atmosphere/model.h"
#include "atmosphere/Cloud.h"
#include "Lightings/PointLight.h"
#include "ParticleSystem/ParticleMaster.h"

#include "WorleyNoise.h"

#include <memory>
#include <thread>

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

	//Utility
	void SwitchIsRenderingText() { isRenderingText = !isRenderingText; }

private:
	//Render Objects
	RenderObject*		object			= nullptr;
	RenderObject*		trajectory		= nullptr;
	RenderObject*		skybox			= nullptr;

	//The FBO contains the result of Rasterization rendering.
	std::unique_ptr<FrameBuffer> renderFBO;

	//Coordinate related
	Camera*		camera					= nullptr;
	Matrix4		projMatrix;
	Matrix4		modelMatrix;

	//Lightings
	PointLight*			pointLight1		= nullptr;

	//Particle System
	ParticleMaster*		particleMaster	= nullptr;

	//Temp creations
	void CreateSkybox();
	void CreateTrajectory();

	//Rendering
	void renderObject();
	void renderSkyBox();

	//For Cloud
	std::unique_ptr<atmosphere::Cloud> cloudModel;
	RenderObject cloudShader;
	void CreateCloud();
	void RenderCloud();

	//For atmospheic scattering
	std::unique_ptr<atmosphere::Model> atmosphereScattering;
	RenderObject atmosphereScatteringShader;
	//I port the demo by https://ebruneton.github.io/precomputed_atmospheric_scattering/
	//into these two methods (model initialization and rendering)
	void CreateAtmosphericScatteringModel();
	void RenderAtmosphericScatteringModel();

	//Utility
	const float renderFrames = 1000.f / 60.f;
	float oneFramePerMilliSecond = 0;
	float oneFramePerSecond;
	std::size_t frameCount = 0;
	std::size_t fps = 0;
	Debug debugger;
	TextRenderer textRenderer;
	bool isRenderingText = true;
	void RenderText();
	

	//Temp
	GLuint noiseTex;
	void noiseGeneration();

public:
	//Save the initial frame in "demo/screenshot.jpg"
	void ScreenShot(std::string filename = "Screenshot");
};


