#pragma once

#include "ParticleSystem.h"
#include <vector>
#include <unordered_map>
#include <memory>

class ParticleMaster
{
public:
	// For the convenience of each particle system to access objects like camera and projection matrix.

	ParticleMaster();
	~ParticleMaster();

	void AddSystem(ParticleSystem* p, string shaderName = "Basic", string textureName = "Cosmic");
	void RemoveSystem();

	void Update(float dt);
	void Render();

	auto GetParticleSystemSize() { return particleSystemArray.size(); }
	
protected:
	vector<ParticleSystem*> particleSystemArray;

	unordered_map<string, shared_ptr<Shader>>	shaders;
	unordered_map<string, shared_ptr<Texture>>	textures;
};

