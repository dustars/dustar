#pragma once

//It's better to use define?
static const float		PI = 3.14159265358979323846f;

//It's pi...divided by 360.0f!
static const float		PI_OVER_360 = PI / 360.0f;

//Radians to degrees
static inline double RadToDeg(const double deg) {
	return deg * 180.0 / PI;
};

//Degrees to radians
static inline double DegToRad(const double rad) {
	return rad * PI / 180.0;
};

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))