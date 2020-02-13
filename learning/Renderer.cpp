#include "Renderer.h"

Renderer::Renderer(Window& parent) : RenderBase(parent), framesPerSecond(0), oneSecond(0) {

	object = new RenderObject();

	object->GetTransform()->CreateRotatingCube(startingTime);
	object->GetMesh()->CreateCube();

	camera = new Camera();

	particle = new ParticleSystem();

	init = true;
}

Renderer::~Renderer(void) {
	delete object;
}

void Renderer::Update(float dt) {
	camera->UpdateCamera(dt);
	FPSCalculation(dt);
	particle->ParticleUpdate(dt);
	Render();
}

void Renderer::Render() {
	const GLfloat backgroundColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, backgroundColor);

	//renderObject();
	renderParticle();
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
	::SwapBuffers(deviceContext);
}

void Renderer::renderParticle()
{
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particle->vbo[ParticleSystem::BASEMESH_BUFFER]);
	glVertexAttribPointer(
		0, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particle->vbo[ParticleSystem::CENTER_BUFFER]);
	glVertexAttribPointer(
		1, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : x + y + z + size => 4
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particle->vbo[ParticleSystem::COLOR_BUFFER]);
	glVertexAttribPointer(
		2, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE, // type
		GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0, // stride
		(void*)0 // array buffer offset
	);

	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad -> 1

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
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