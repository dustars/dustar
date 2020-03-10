#pragma once
#include "Mesh.h"


class Trajectory : public Mesh
{
public:
	Trajectory() {}
	Trajectory(Vector3 dir = Vector3(0, 0, -1), float force = 10.f );
	~Trajectory() {}

	void UpdateDirection(float degree);

private:
	//How many points to draw?
	void CreateTrajectory();

	Vector3 dir;
	float force = 0;

	static constexpr float gravity = 9.8f;
	static const int pointInterval = 10;
	static const int totalPoints = 30;
};

