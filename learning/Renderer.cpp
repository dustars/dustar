#include "Renderer.h"

Renderer::Renderer(Window& parent) : RenderBase(parent), framesPerSecond(0), oneSecond(0), startingTime(0),
	object(nullptr), particleMaster(nullptr)
{
	// Initialize the basics ( later move to a function )
	camera = new Camera(-50,-110,Vector3(0,500,200.f));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	// That experimental quad
	object = new RenderObject();
	if (!object->SetShader("shader/HeightMapVShader.glsl", "shader/HeightMapFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	if (!object->GetTexture()->SetTexture("../assets/Textures/Barren Reds.jpg")) {
		cout << "Texture set up failed!" << endl;
	}

	//object->GetMesh()->CreateTriangle();
	object->SetMesh(new HeightMap(6, 3, 0.4, 500, 500));

	//CreateParticle();

	skybox = new RenderObject();
	if (!skybox->SetShader("shader/SkyBoxVShader.glsl", "shader/SkyBoxFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	skybox->GetTexture()->CreateCubeMap("../assets/Skybox/bluecloud_rt.jpg",
										"../assets/Skybox/bluecloud_lf.jpg",
										"../assets/Skybox/bluecloud_up.jpg",
										"../assets/Skybox/bluecloud_dn.jpg",
										"../assets/Skybox/bluecloud_bk.jpg",
										"../assets/Skybox/bluecloud_ft.jpg");
	skybox->GetMesh()->CreateSkyBox();

	init = true;
	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer(void) {
	delete camera;
	if (object) {
		delete object;
	}
	if (skybox) {
		delete skybox;
	}
	if (particleMaster) {
		delete particleMaster;
	}
}

void Renderer::Update(float dt) {
	camera->UpdateCamera(dt*0.5f);
	if (particleMaster) {
		particleMaster->Update(dt);
	}
	////////////
	// Render //
	////////////
	Render();

	// Utility
	FPSCalculation(dt);
}

void Renderer::Render() {
	const GLfloat backgroundColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (skybox) {
		renderSkyBox();
	}
	
	if (object) {
		renderObject();
	}

	if (particleMaster) {
		particleMaster->Render();
	}

	auto error = glGetError();
	if (error) {
		cout << "\nError(Code: " << error << "). Please check the program." << endl;
	}

	::SwapBuffers(deviceContext);
}

void Renderer::renderObject()
{
	glUseProgram(object->GetShader()->GetProgram());
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&camera->BuildViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	object->Draw();
	glUseProgram(0);
}

void Renderer::renderSkyBox()
{
	glUseProgram(skybox->GetShader()->GetProgram());
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glUniformMatrix4fv(glGetUniformLocation(skybox->GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE,(float*)&camera->BuildViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(skybox->GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetTexture()->GetTexture());
	
	skybox->GetMesh()->Draw();

	glDepthMask(GL_TRUE);
	glUseProgram(0);
}

void Renderer::FPSCalculation(float dt) {
	oneSecond += dt * 0.001f;
	framesPerSecond = framesPerSecond + 1;
	if (oneSecond > 1.0f) {
		cout << "Current FPS: " << framesPerSecond << endl;
		oneSecond = 0;
		framesPerSecond = 0;
	}
}

void Renderer::CreateParticle()
{
	Shader* particleShader = new Shader("shader/ParticleBaseVShader.glsl", "shader/ParticleBaseFShader.glsl");
	if (!particleShader->LinkProgram()) {
		cout << "Shader set up failed!" << endl;
	}

	particleMaster = new ParticleMaster();
	particleMaster->AddSystem(new ParticleSystemBase(projMatrix, "../assets/Textures/cosmic.png", 4,
		camera, particleShader, 1, {350,100,350 }, 5.f, 2, 10));
}
