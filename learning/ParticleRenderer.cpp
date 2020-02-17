#include "ParticleRenderer.h"


ParticleRenderer::ParticleRenderer(Matrix4 projMatrix, Camera* camera) : projMatrix(projMatrix), camera(camera){

	particleShader = new Shader("shader/ParticleVShader.glsl", "shader/BasicColorFShader.glsl");
	if (!particleShader->LinkProgram()) {
		cout << "Shader set up failed!" << endl;
	}

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo[BASEMESH_BUFFER]);
	const GLfloat quad[] = {
		 -0.5f, -0.5f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f,
		 -0.5f, 0.5f, 0.0f, 1.0f,
		 0.5f, 0.5f, 0.0f, 1.0f,
	};
	glNamedBufferStorage(vbo[BASEMESH_BUFFER], sizeof(quad), quad, 0);
	glVertexArrayVertexBuffer(vao, BASEMESH_BUFFER, vbo[BASEMESH_BUFFER], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vao, BASEMESH_BUFFER, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, BASEMESH_BUFFER, BASEMESH_BUFFER);
}

ParticleRenderer::~ParticleRenderer()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(MAX_BUFFER, vbo);
	delete particleShader;
}

void ParticleRenderer::RenderParticle()
{
	glUseProgram(particleShader->GetProgram());
	glBindVertexArray(vao);
	glEnableVertexAttribArray(BASEMESH_BUFFER);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	Matrix4 viewMatrix = camera->BuildViewMatrix();

	//iterate every particle and update their modelview matrix
	for (auto&& element : Particle::ParticleList) {
		UpdateMatrix(element, viewMatrix);
		glUniform3fv(glGetUniformLocation(particleShader->GetProgram(), "color"), 1, (float*)&element.GetColor());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDepthMask(true);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(BASEMESH_BUFFER);
	glBindVertexArray(0);
	glUseProgram(0);
}

void ParticleRenderer::UpdateMatrix(Particle& p, const Matrix4& viewMatrix)
{
	Matrix4 modelMatrix;
	modelMatrix = modelMatrix * Matrix4::Translation(p.GetPosition());

	//We don't need rotation component in the matrix, just make it transpose of view matrix.
	modelMatrix.values[0] = viewMatrix.values[0];
	modelMatrix.values[1] = viewMatrix.values[4];
	modelMatrix.values[2] = viewMatrix.values[8];
	modelMatrix.values[4] = viewMatrix.values[1];
	modelMatrix.values[5] = viewMatrix.values[5];
	modelMatrix.values[6] = viewMatrix.values[9];
	modelMatrix.values[8] = viewMatrix.values[2];
	modelMatrix.values[9] = viewMatrix.values[6];
	modelMatrix.values[10] = viewMatrix.values[10];

	modelMatrix = modelMatrix * Matrix4::Rotation((float)DegToRad(p.GetRotation()), { 0,0,1 });
	modelMatrix = modelMatrix * Matrix4::Scale({ p.GetScale(), p.GetScale(), p.GetScale() });

	Matrix4 TransformMatrix = projMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(particleShader->GetProgram(), "TransformMatrix"), 1, GL_FALSE, (float*)&TransformMatrix);
}