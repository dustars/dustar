#include "ParticleMaster.h"


ParticleMaster::ParticleMaster()
{
	/////// Create Shaders
	auto particleShader = make_shared<Shader>("shader/ParticleBaseVShader.glsl", "shader/ParticleBaseFShader.glsl");
	if (!particleShader->LinkProgram()) {
		cout << "Shader set up failed!" << endl;
	}
	shaders["core"] = particleShader;

	/////// Create Textures
	auto texture = make_shared<Texture>();
	if (!texture->SetTexture("../assets/Textures/cosmic.png", 4)) {
		cout << "Texture Set up failed!" << endl;
	}
	textures["Cosmic"] = texture;

	auto texture1 = make_shared<Texture>();
	if (!texture1->SetTexture("../assets/Textures/particleAtlas.png", 4)) {
		cout << "Texture Set up failed!" << endl;
	}
	textures["RedDot"] = texture1;

	auto texture2 = make_shared<Texture>();
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
	for (const auto& element : particleSystemArray) {
		element->Update(dt);
	}
}

void ParticleMaster::Render()
{
	if (!particleSystemArray.size()) {
		return;
	}

	for (const auto& element : particleSystemArray) {
		element->Render();
	}
}
