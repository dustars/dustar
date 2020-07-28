#pragma once

#include "Window.h"
#include "math/CommonOps.h"

class Camera
{
public:
	Camera(float pitch = 0, float yaw = 0, Vector3 position = Vector3(0, 0, 0));
	~Camera(void) {};

	void UpdateCamera(float msec);

	Matrix4 BuildViewMatrix();

	Vector3	GetPosition() const { return position; }
	void	SetPosition(Vector3 val) { position = val; }

	float	GetYaw()   const { return yaw; }
	void	SetYaw(float y) { yaw = y; }

	float	GetPitch() const { return pitch; }
	void	SetPitch(float p) { pitch = p; }

private:
	float	yaw;
	float	pitch;
	Vector3 position;

	//Camera update speed
	static constexpr float UPDATE_MULTIPLIER = 1.f;
};

