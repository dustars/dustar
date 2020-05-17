#pragma once
#include "../basic/math/Math.h"

class Transform
{
public:
	Transform() {}
	~Transform() {}

	Matrix4& GetModelMatrix() { return modelMatrix; }
	void SetTransform(const Matrix4& tran) { modelMatrix = tran; }
	void SetTransform(const Matrix4&& tran) { modelMatrix = tran; }

	void CreateRotatingCube(float rand);

protected:
	Matrix4 modelMatrix;
};

