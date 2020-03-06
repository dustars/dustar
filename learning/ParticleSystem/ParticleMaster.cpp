#include "ParticleMaster.h"

ParticleMaster::ParticleMaster()
{
	Shader* particleShader = new Shader("shader/ParticleBaseVShader.glsl", "shader/ParticleBaseFShader.glsl");
	if (!particleShader->LinkProgram()) {
		cout << "Shader set up failed!" << endl;
	}

	Shaders["Basic"] = particleShader;
}

ParticleMaster::~ParticleMaster()
{
}

void ParticleMaster::AddSystem(ParticleSystem* p, string shaderName)
{
	p->particleShader = Shaders[shaderName];
	particleSystemArray.push_back(*p);
}

void ParticleMaster::RemoveSystem()
{
}

void ParticleMaster::Update(float dt)
{
	if (!particleSystemArray.size()) {
		return;
	}
	for (auto& element : particleSystemArray) {
		element.Update(dt);
	}
}

void ParticleMaster::Render()
{
	if (!particleSystemArray.size()) {
		return;
	}

	for (auto& element : particleSystemArray) {
		element.Render();
	}
}
