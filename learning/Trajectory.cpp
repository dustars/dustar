#include "Trajectory.h"

Trajectory::Trajectory()
{
	renderType = GL_LINE_STRIP;

	numOfVertices = totalPoints;

	CreateTrajectory();

	BufferDataToGPU();
}

//How it works:
//Given the current dir and force, calculate each point in an interval of time t.
void Trajectory::CreateTrajectory()
{
	float t = 0;
	Vector3 vel = dir * force;

	for (auto i = 1; i <= totalPoints; ++i) {

		t += 0.25f; // Not necessarily +1, could be +0.1.

		float x = currentPos.x + vel.x * t;
		float z = currentPos.z + vel.z * t;
		float y = currentPos.y + ((t * t * gravity) / (2 * weight));

		// Not push_back, but actually change the element inside
		position.push_back({ x, y, z });

		// Temp color specification
		if (i % 3 == 0) {
			color.push_back({ 1.f, 0,0 });
		}
		if (i % 3 == 1) {
			color.push_back({ 0, 1.f,0 });
		}
		if (i % 3 == 2) {
			color.push_back({ 0, 0,1.f });
		}

	}
}

void Trajectory::CreateTrajectory(Vector3 vel)
{
	float t = 0;
	for (auto i = 1; i <= totalPoints; ++i) {

		t += 0.25f; // Not necessarily +1, could be +0.1.

		float x = currentPos.x + vel.x * t;
		float z = currentPos.z + vel.z * t;
		float y = currentPos.y + ((t * t * gravity) / (2 * weight));

		// Not push_back, but actually change the element inside
		position.at(i - 1) = { x, y, z };
	}
}

void Trajectory::Update(float dt)
{
	rotateDegree += 0.0002f * 1000.f / dt;
	if (rotateDegree > 2*PI) rotateDegree = 0;
	dir.x = sin(rotateDegree);
	dir.z = cos(rotateDegree);
	dir.Normalise();
	//Change dir
	CreateTrajectory(dir * force);
	UpdateDataToGPU();
}
