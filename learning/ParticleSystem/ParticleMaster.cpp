#include "ParticleMaster.h"
#include <algorithm>

ParticleMaster::ParticleMaster()
{
	Shader* particleShader = new Shader("shader/ParticleBaseVShader.glsl", "shader/ParticleBaseFShader.glsl");
	if (!particleShader->LinkProgram()) {
		cout << "Shader set up failed!" << endl;
	}
	shaders["Basic"] = particleShader;

	Texture* texture = new Texture();
	if (!texture->SetTexture("../assets/Textures/cosmic.png", 4)) {
		cout << "Texture Set up failed!" << endl;
	}
	textures["Cosmic"] = texture;

	Texture* texture1 = new Texture();
	if (!texture1->SetTexture("../assets/Textures/particleAtlas.png", 4)) {
		cout << "Texture Set up failed!" << endl;
	}
	textures["RedDot"] = texture1;

	Texture* texture2 = new Texture();
	if (!texture2->SetTexture("../assets/Textures/container.jpg", 1)) {
		cout << "Texture Set up failed!" << endl;
	}
	textures["Container"] = texture2;
}

ParticleMaster::~ParticleMaster()
{
	for (auto& ele : particleSystemArray) {
		delete ele;
	}

	for (auto i = shaders.begin(); i != shaders.end(); ++i) {
		delete i->second;
	}
	
	for (auto i = textures.begin(); i != textures.end(); ++i) {
		delete i->second;
	}
}

void ParticleMaster::AddSystem(ParticleSystem* p, string shaderName, string textureName)
{
	p->particleShader = shaders[shaderName];
	p->texture = textures[textureName];

	particleSystemArray.push_back(p); // Why only pointers
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
		element->Update(dt);
	}
}

void ParticleMaster::Render()
{
	if (!particleSystemArray.size()) {
		return;
	}

	for (auto& element : particleSystemArray) {
		element->Render();
	}
}
