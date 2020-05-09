#pragma once
#include "../basic/math/Matrix4.h"

class Transform
{
public:
	Transform() {}
	~Transform() {}

	inline Matrix4& GetModelMatrix() { return modelMatrix; }
	inline void SetTransform(Matrix4 tran) { modelMatrix = tran; }

	void CreateRotatingCube(float rand);

protected:
	Matrix4 modelMatrix;
};

