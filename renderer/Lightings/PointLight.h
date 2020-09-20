#include "Light.h"

#include "../core/math/Math.h"

class PointLight : public Light {
public:
	PointLight(Vector3 p = Vector3(), Vector4 c = Vector4()) : position(p), color(c) {}
	~PointLight() override {}

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	Vector4 GetColor() const { return color; }
	void SetColor(const Vector4& val) { color = val; }

protected:
	Vector3 position;
	Vector4 color;
};