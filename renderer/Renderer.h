/*
	Description:
	--------------------------------------------------------------------------------------
	Everything happens in here....
	Creating Objects, update and of course rendering.

	About Controls:
	--------------------------------------------------------------------------------------
	WASD:	movement.
	P:		ScreenShot
	T:		Enable/Disable text rendeing.
	Ctrl+C:	Enable/Disable cursor and ImGUI (essentially serves as debugger)
	Press mouse left button + move mouse: move sun direction.

	TODO:
	--------------------------------------------------------------------------------------
	1. Create a manager class or at least a vector to contain all those RenderObjects.
	2. Use unique_ptr for all pointer type data members?
	3. It's kinda awkward to have both text rendering and Dear ImGUI, because the latter
	can replace the former easily. What am I gonna do with the text rendering then?
*/

#pragma once
#include "Configuration.h"

//Cores
#include "RenderBase.h"
#include "RenderObject.h"
#include "Camera.h"
#include "FrameBuffer.h"

//Features (Objects)
#include "HeightMap.h"
#include "Trajectory.h"
#include "atmosphere/model.h"
#include "atmosphere/Cloud.h"
#include "Lightings/PointLight.h"
#include "ParticleSystem/ParticleMaster.h"

//Tools
#include "tools/Debug.h"
#include <imgui.h>
#include "tools/Imgui/imgui_impl_opengl3.h"
#include "tools/Imgui/imgui_impl_win32.h"

#include <memory>
#include <thread>

//temp parameters for terrain, need to be removed
constexpr auto MAPWIDTH = 500;
constexpr auto MAPLENGTH = 500;

constexpr auto SHADOWWIDTH = 1024;
constexpr auto SHADOWHEIGHT = 1024;

//Fina a way to get rid of this forward declaration.
class ParticleMaster;

class Renderer : public RenderBase
{
public:
	//Initialization.
	Renderer(Window& parent);
	~Renderer() override;

	//The current entrance to rendering loop
	void Update(float dt) override;
	//Get called by update()
	void Render() override;

	Camera* GetCamera()		const{ return camera; }
	Matrix4 GetProjMatrix() const{ return projMatrix; }

private:
	//FPS, text, ImGui, etc.
	void UtilityUpdate();
	void UtilityRender();

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
	Matrix4		cameraMatrix;

	//Lightings
	std::unique_ptr<PointLight> light1;

	//Shadow Mapping
	std::unique_ptr<FrameBuffer> shadowFBO;
	RenderObject shadowMappingShader;
	Matrix4 lightMatrix;
	void CreateShadowMap();
	void RenderShadowMap();

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
	void ImGUIInit(Window& parent);
	void RenderImGUI();
	void UpdateControl(float msec); //temp

//Some public utility methods may be helpful.
public:
	//Save the initial frame in "demo/screenshot.jpg"
	void ScreenShot(std::string filename = "Screenshot");

	//Switch on/off to render text.
	void SwitchIsRenderingText() { isRenderingText = !isRenderingText; }
};


