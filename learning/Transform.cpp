#include "Transform.h"

void Transform::CreateRotatingCube(float rand)
{
	float f = (float)rand * (float)PI * 0.1f;
	modelMatrix =
		Matrix4::Translation(Vector3(0.0f, 0.0f, -4.0f)) *
		Matrix4::Translation(Vector3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f)) *
		Matrix4::Rotation((float)rand * 45.0f, Vector3(0.0f, 1.0f, 0.0f)) *
		Matrix4::Rotation((float)rand * 81.0f, Vector3(1.0f, 0.0f, 0.0f));
}
