#include "Light.h"

#include "../basic/math/Vector4.h"

class PointLight : public Light {
public:
	PointLight(Vector4 p = Vector4(), Vector4 c = Vector4()) : position(p), color(c) {}
	~PointLight() override {}

	Vector4 GetPosition() const { return position; }
	void SetPosition(Vector4 val) { position = val; }

	Vector4 GetColor() const { return color; }
	void SetColor(Vector4 val) { color = val; }

protected:
	Vector4 position;
	Vector4 color;
};