#include "ParticleMaster.h"


ParticleMaster::ParticleMaster(){


}

ParticleMaster::~ParticleMaster()
{
	if (!particleSystemArray->size()) {
		return;
	}
	for (unsigned int i = 0; i < particleSystemArray->size(); ++i) {
		delete particleSystemArray->at(i);
	}
}

void ParticleMaster::AddSystem(ParticleSystemBase* p)
{
	particleSystemArray->push_back(p);
}

void ParticleMaster::RemoveSystem()
{

}

void ParticleMaster::Update(float dt)
{
	if (!particleSystemArray->size()) {
		return;
	}
	for (unsigned int i = 0; i < particleSystemArray->size(); ++i) {
		particleSystemArray->at(i)->Update(dt);
	}
}

void ParticleMaster::Render()
{
	if (!particleSystemArray->size()) {
		return;
	}
	for (unsigned int i = 0; i < particleSystemArray->size(); ++i) {
		particleSystemArray->at(i)->Render();
	}
}

void ParticleMaster::DeleteParticleMaster()
{
}

