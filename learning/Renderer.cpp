#include "Renderer.h"

Renderer::Renderer(Window& parent) : RenderBase(parent), framesPerSecond(0), oneSecond(0), startingTime(0){

	//object = new RenderObject();
	//if (!object->SetShader("shader/TriangleVShader.glsl", "shader/TriangleFShader.glsl")) {
	//	cout << "Shader set up failed!" << endl;
	//}
	//if (!object->GetTexture()->SetTexture("../assets/Textures/container.jpg")) {
	//	cout << "Texture set up failed!" << endl;
	//}

	//object->GetMesh()->CreateTriangle();

	camera = new Camera();
	Shader* particleShader = new Shader("shader/ParticleBaseVShader.glsl", "shader/ParticleBaseFShader.glsl");
	if (!particleShader->LinkProgram()) {
		cout << "Shader set up failed!" << endl;
	}

	projMatrix = Matrix4::Perspective(0.1f, 1000.0f, (float)width / (float)height, 50.0f);

	particleMaster = new ParticleMaster();
	particleMaster->AddSystem(new ParticleSystemBase(projMatrix, "../assets/Textures/star.jpg",
		camera, particleShader, 5, { 0,0,-50 }, 3.f, 2, 7));

	init = true;
}

Renderer::~Renderer(void) {
	delete object;
	delete camera;
	delete particleMaster;
}

void Renderer::Update(float dt) {
	camera->UpdateCamera(dt*0.1f);
	particleMaster->Update(dt);

	////////////
	// Render //
	////////////
	Render();

	// Utility
	FPSCalculation(dt);
}

void Renderer::Render() {
	const GLfloat backgroundColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, backgroundColor);

	//renderObject();
	particleMaster->Render();
	::SwapBuffers(deviceContext);
}

void Renderer::renderObject()
{
	glUseProgram(object->GetShader()->GetProgram());
	glBindVertexArray(object->GetMesh()->GetVAO());
	glBindTexture(GL_TEXTURE_2D, object->GetTexture()->GetTexture());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Update modelMatrix
	//object->GetTransform()->CreateRotatingCube(startingTime);
	//glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "modelViewMatrix"), 1, GL_FALSE,
	//	(float*)&object->GetTransform()->GetModelMatrix());
	//Matrix4 projMatrix = Matrix4::Perspective(0.1f, 1000.0f, (float)width / (float)height, 50.0f);
	//glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "projMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "viewMatrix"), 1, GL_FALSE, (float*)&camera->BuildViewMatrix());


	glDrawArrays(object->GetMesh()->GetType(), 0, object->GetMesh()->GetNumOfVertices());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
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