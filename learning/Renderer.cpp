#include "Renderer.h"

Renderer::Renderer(Window& parent) : RenderBase(parent), framesPerSecond(0), oneSecond(0) {

	//object = new RenderObject();
	//object->GetTransform()->CreateRotatingCube(startingTime);
	//object->GetMesh()->CreateCube();

	camera = new Camera();

	Matrix4 projMatrix = Matrix4::Perspective(0.1f, 1000.0f, (float)width / (float)height, 50.0f);

	particleRenderer = new ParticleRenderer(projMatrix, camera);

	Particle::AddParticle({ 0,0,0 }, {0, 30, 0}, 0 ,1, 1, 4);

	init = true;
}

Renderer::~Renderer(void) {
	delete object;
}

void Renderer::Update(float dt) {
	camera->UpdateCamera(dt);
	ParticleRenderer::ParticleUpdate(dt * 0.001f);
	Render();

	// Utility
	FPSCalculation(dt);
}

void Renderer::Render() {
	const GLfloat backgroundColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, backgroundColor);

	//renderObject();
	particleRenderer->RenderParticle();
	::SwapBuffers(deviceContext);
}

void Renderer::renderObject()
{
	glUseProgram(object->GetShader()->GetProgram());
	glBindVertexArray(object->GetMesh()->GetVAO());

	// Update modelMatrix
	object->GetTransform()->CreateRotatingCube(startingTime);
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "modelViewMatrix"), 1, GL_FALSE,
		(float*)&object->GetTransform()->GetModelMatrix());
	Matrix4 projMatrix = Matrix4::Perspective(0.1f, 1000.0f, (float)width / (float)height, 50.0f);
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "projMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "viewMatrix"), 1, GL_FALSE, (float*)&camera->BuildViewMatrix());

	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, object->GetMesh()->GetNumOfVertices());

	glDisableVertexAttribArray(0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Renderer::FPSCalculation(float dt) {
	oneSecond += dt * 0.001;
	framesPerSecond = framesPerSecond + 1;
	if (oneSecond > 1.0f) {
		cout << "Current FPS: " << framesPerSecond << endl;
		oneSecond = 0;
		framesPerSecond = 0;
	}
}