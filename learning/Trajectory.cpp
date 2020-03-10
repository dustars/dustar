#include "Trajectory.h"

Trajectory::Trajectory(Vector3 d, float f) :
	dir(d),
	force(f)
{
	renderType = GL_LINE_STRIP;

	numOfVertices = totalPoints;

	CreateTrajectory();
	BufferDataToGPU();
}

void Trajectory::CreateTrajectory()
{
	dir

	for (auto i = 1; i <= totalPoints; ++i) {
		position.push_back();
	}
}

void Trajectory::UpdateDirection(float degree)
{
}
