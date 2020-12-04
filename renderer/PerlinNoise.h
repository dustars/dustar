/*
	Description:
	Perlin noise class.

	Copyright:
	This implementation is ported from http://mrl.nyu.edu/~perlin/noise/,
	which is the original Java version authored by Ken Perlin himself.
*/

#pragma once
#include <vector>

class PerlinNoise {
	// The permutation vector
	std::vector<int> p;
public:
	// Initialize with the reference values for the permutation vector
	PerlinNoise();
	// Generate a new permutation vector based on the value of seed
	PerlinNoise(unsigned int seed);
	// Get a noise value, for 2D images z can have any value
	double noise(double x, double y, double z);
	double FBMPerlin(double x, double y, double z, int octaves, double lacunarity = 2.0, double gain = 0.707);
private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
};