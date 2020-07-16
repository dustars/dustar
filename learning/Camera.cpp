#include "Camera.h"

Camera::Camera(void) {
	pitch = 0.0f;
	yaw = 0.0f;
}

Camera::Camera(float pitch, float yaw, Vector3 position) {
	this->pitch = pitch;
	this->yaw = yaw;
	this->position = position;
}

void Camera::UpdateCamera(float msec) {
	//Update the mouse by how much
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	//msec *= 30.0f;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec) / UPDATE_MULTIPLIER;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec) / UPDATE_MULTIPLIER;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position += (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec) / UPDATE_MULTIPLIER;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position -= (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec) / UPDATE_MULTIPLIER;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += msec / UPDATE_MULTIPLIER;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y -= msec / UPDATE_MULTIPLIER;
	}
}

Matrix4 Camera::BuildViewMatrix() {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};