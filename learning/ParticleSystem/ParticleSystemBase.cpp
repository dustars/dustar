#include "ParticleSystemBase.h"

ParticleSystemBase::ParticleSystemBase(Matrix4 projMatrix, string texFile, Camera* camera, Shader* shader,
	unsigned int number, Vector3 position, float life, unsigned int variation, int initialForce):
	projMatrix(projMatrix), camera(camera), particleShader(shader), 
	number(number), position(position), life(life), variation(variation), initialForce(initialForce), texture(nullptr)
{
	// Set Texture
	texture = new Texture();
	if (!texture->SetTexture(texFile)) {
		cout << "Texture Set up failed!" << endl;
		delete texture;
		return;
	}

	// Shape Define
	const float quad[] = {
	 -1.f, -1.f, 0.0f, 1.0f,
	 1.f, -1.f, 0.0f, 1.0f,
	 -1.f, 1.f, 0.0f, 1.0f,
	 1.f, 1.f, 0.0f, 1.0f,
	};
	SetShape(quad);

	// Create VAO for quad mesh
	glCreateVertexArrays(1, &vao);
	glCreateBuffers(MAX_BUFFER, vbo);
	
	glNamedBufferStorage(vbo[BASEMESH_BUFFER], sizeof(quad), quad, 0);
	glVertexArrayVertexBuffer(vao, BASEMESH_BUFFER, vbo[BASEMESH_BUFFER], 0, sizeof(Vector4));
	glVertexArrayAttribFormat(vao, BASEMESH_BUFFER, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, BASEMESH_BUFFER, BASEMESH_BUFFER);

	const float texCoords[] = {
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 0.0f, 1.0f,
	 1.0f, 1.0f,
	};

	glNamedBufferStorage(vbo[TEXTURE_BUFFER], sizeof(texCoords), texCoords, 0);
	glVertexArrayVertexBuffer(vao, TEXTURE_BUFFER, vbo[TEXTURE_BUFFER], 0, sizeof(Vector2));
	glVertexArrayAttribFormat(vao, TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, TEXTURE_BUFFER, TEXTURE_BUFFER);
}

ParticleSystemBase::~ParticleSystemBase()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(MAX_BUFFER, vbo);
	delete particleShader;
	delete texture;
}

void ParticleSystemBase::Update(float dt)
{
	//Generate new particles per frame
	EmitParticles();

	//Update each particle
	for (auto i = particleList.begin(); i != particleList.end(); ++i) {
		bool ifAlived = i->Update(dt);
		i->distanceFromCamera = (camera->GetPosition() - i->position).Length();
		if (!ifAlived) {
			i = particleList.erase(i);
			if (i == particleList.end()) {
				break;
			}
		}
	}
	//Sort the list at the end of each update
	//I literally don't understand the time complexity for this one, and how it affects the overall performance
	//Need to check out in the future
	auto sortDis = [](const Particle& first, const Particle& second)
	{ return (first.distanceFromCamera > second.distanceFromCamera) ? true : false; };
	particleList.sort(sortDis);
}

void ParticleSystemBase::Render()
{
	if (!particleShader) {
		cout << "Particle System doesn't have a shader!" << endl;
		return;
	}
	glUseProgram(particleShader->GetProgram());
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
	glEnableVertexAttribArray(BASEMESH_BUFFER);
	glEnableVertexAttribArray(TEXTURE_BUFFER);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	Matrix4 viewMatrix = camera->BuildViewMatrix();

	//iterate every particle and update their modelview matrix
	for (auto&& element : particleList) {
		UpdateMatrix(element, viewMatrix);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDepthMask(true);
	glDisable(GL_BLEND);
	glEnableVertexAttribArray(TEXTURE_BUFFER);
	glDisableVertexAttribArray(BASEMESH_BUFFER);
	glBindVertexArray(0);
	glUseProgram(0);
}

void ParticleSystemBase::SetShape(const float shape[16]) {
	for (int i = 0; i <= 15; ++i) {
		shapeArray[i] = shape[i];
	}
}

void ParticleSystemBase::EmitParticles()
{
	for (unsigned int i = 0; i < number; ++i) {
		// random velocity of unit circle
		float dirX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;
		float dirZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;
		Vector3 velocity(dirX, 0, dirZ);
		velocity.Normalise();

		if (!variation) {
			float randY = rand() % (variation + 1);
		}
		velocity.x *= initialForce;
		velocity.y = variation * initialForce;
		velocity.z *= initialForce;

		Particle newP(position, velocity, life, 0, 1, 1);
		particleList.push_back(newP);
	}
}

void ParticleSystemBase::UpdateMatrix(Particle& p, const Matrix4& viewMatrix)
{
	Matrix4 modelMatrix;
	modelMatrix = modelMatrix * Matrix4::Translation(p.position);

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

	modelMatrix = modelMatrix * Matrix4::Rotation((float)DegToRad(p.rotation), { 0,0,1 });
	modelMatrix = modelMatrix * Matrix4::Scale({ p.scale, p.scale, p.scale });

	Matrix4 TransformMatrix = projMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(particleShader->GetProgram(), "TransformMatrix"), 1, GL_FALSE, (float*)&TransformMatrix);
}