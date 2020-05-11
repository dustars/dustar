/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once

#include "Vector3.h"

class Vector4	{
public:
	Vector4(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w){
	}
	Vector4(const Vector3& v3, float w): x(v3.x), y(v3.y), z(v3.z), w(w) {}

	Vector3 ToVector3() {
		return Vector3(x, y, z);
	}

	~Vector4(void){}

	float x;
	float y;
	float z;
	float w;
};

