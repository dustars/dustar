/*
	Description:
	Camera Class and temporary control class (need to be removed to a single file later)

	Aparting from having all necessary data for a camera class, this file also contains
	some bool values to control gui, cursor, or maybe something added later. In addition,
	it has zenith and azimuth angles of the sun to calculate the direction of light.
*/

#pragma once
#include "Window.h"
#include "math/CommonOps.h"

class Camera
{
public:
	Camera(Window& w, float pitch = 0, float yaw = 360, Vector3 position = Vector3(0, 0, 0));
	~Camera(void) {};

	void UpdateCamera(float msec);

	Matrix4 BuildViewMatrix();

	Vector3	GetPosition() const { return position; }
	void	SetPosition(Vector3 val) { position = val; }

	float	GetYaw()   const { return yaw; }
	void	SetYaw(float y) { yaw = y; }

	float	GetPitch() const { return pitch; }
	void	SetPitch(float p) { pitch = p; }
	
	float GetSunZenithDegree() const { return sunZenith; }
	float GetSunAzimuthDegree() const {return sunAzimuth; }
	Vector3 GetSunDirection() const {
		float zenith = DegToRad(sunZenith);
		float azimuth = DegToRad(sunAzimuth);
		return Vector3(sin(azimuth) * sin(zenith),cos(zenith),cos(azimuth) * sin(zenith));
	}

	bool GetShowGUI() const { return showGUI; }

private:
	//Have the access to the window so that some states can be changed, e.g. show cursor.
	Window& w;

	float	yaw;
	float	pitch;
	Vector3 position;

	void CameraControlUpdate(float msec);
	void CursorUpdate(float msec);

	bool showCursor = false;
	bool showGUI = false;

	float sunZenith = 87.64f;
	float sunAzimuth = 342.5f;

	//Camera update speed
	static constexpr float UPDATE_MULTIPLIER = 10.f;
};

