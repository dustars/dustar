#pragma once

#include "Window.h"
#include "../basic/math/Math.h"

constexpr float UPDATE_MULTIPLIER = 100;

class Camera
{
public:
	Camera(void);
	Camera(float pitch, float yaw, Vector3 position);

	~Camera(void) {};

	void UpdateCamera(float msec);

	Matrix4 BuildViewMatrix();

	inline Vector3	GetPosition() const { return position; }
	inline void		SetPosition(Vector3 val) { position = val; }

	inline float	GetYaw()   const { return yaw; }
	inline void		SetYaw(float y) { yaw = y; }

	inline float	GetPitch() const { return pitch; }
	inline void		SetPitch(float p) { pitch = p; }

protected:
	float	yaw;
	float	pitch;
	Vector3 position;
};

