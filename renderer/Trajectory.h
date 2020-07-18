#pragma once
#include "Mesh.h"

class Trajectory : public Mesh
{
public:
	Trajectory();
	~Trajectory() override = default;

	void Update(float dt) final;

private:
	//How many points to draw?
	void CreateTrajectory();
	void UpdateTrajectory(Vector3 vel);

	Vector3 currentPos		= { 0,0,0 };
	Vector3 dir				= { 0,0,-1 };
	float force				= 20;
	float weight			= 1.f;

	float rotateDegree = 0; // For rotation

	static constexpr float gravity = -9.8f;
	static const int totalPoints = 30;
};

