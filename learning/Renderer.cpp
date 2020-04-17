#include "Renderer.h"

Renderer::Renderer(Window& parent) : RenderBase(parent)
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

	// Particle System Creation
	ParticleSystem::renderer = this; // Let all particle systems be able to access the resources
	particleMaster = new ParticleMaster();
	//particleMaster->AddSystem(new ParticleSystem(60, { 350,100,350 }, 5.f, 1, 100));
	particleMaster->AddSystem(new ParticleSystem(10, { 300,100,300 }, 5.f, 0, 25, EmitType::TRAJECTORY));

	CreateSkybox();
	CreateTrajectory();

	

	init = true;
	glEnable(GL_DEPTH_TEST);		
}

Renderer::~Renderer(void) {
	delete camera;
	if (object) {
		delete object;
	}
	if (trajectory) {
		delete trajectory;
	}
	if (skybox) {
		delete skybox;
	}
	if (particleMaster) {
		delete particleMaster;
	}
}

void Renderer::Update(float dt) {

	// Todo: Put into the 60 fps loop
	camera->UpdateCamera(dt);

	// Temporary method to limit frame rate at 60FPS
	oneFrame += dt;
	if (oneFrame > renderFrames) {
		////////////////////////
		//  Locations Update  //
		////////////////////////
		if (particleMaster) {
			particleMaster->Update(oneFrame);
		}

		if (trajectory) {
			trajectory->GetMesh()->Update(oneFrame);
		}

		////////////////////////
		// Graphics Rendering //
		////////////////////////
		Render();

		oneFrame = 0;
	}
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

	if (trajectory) {
		glUseProgram(trajectory->GetShader()->GetProgram());
		glUniformMatrix4fv(glGetUniformLocation(trajectory->GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&camera->BuildViewMatrix());
		glUniformMatrix4fv(glGetUniformLocation(trajectory->GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
		glUniformMatrix4fv(glGetUniformLocation(trajectory->GetShader()->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
		trajectory->Draw();
		glUseProgram(0);
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

void Renderer::CreateSkybox()
{
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
	skybox->GetMesh()->CreateQuad();
}

void Renderer::CreateTrajectory()
{
	trajectory = new RenderObject();

	if (!trajectory->SetShader("shader/TriangleVShader.glsl", "shader/TriangleFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	trajectory->SetMesh(new Trajectory());
}
