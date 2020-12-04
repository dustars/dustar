#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//Is there any problems with the binding points?
layout(location = 0) uniform uint maxTableSize = 256; //Currently hardcoded
layout(location = 1) uniform float samplingRate;

layout(binding = 0) buffer permutationTableBlock {
	int permutationTable[512];
};

layout (binding = 0, rgba8ui) uniform uimage2D resultImg;

float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
	return a + t * (b - a);
}

float grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	float u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

void main(void) {

	ivec3 p = ivec3(gl_GlobalInvocationID.xyz);

	vec3 pS = vec3(p) * samplingRate;

	float x = pS.x;
	float y = pS.y;
	float z = 0;	//for 2D, z must be 0

	int X = int(x) & 255;
	int Y = int(y) & 255;
	int Z = int(z) & 255;

	// Find relative x, y,z of point in cube
	x -= int(x);
	y -= int(y);
	z -= int(z);

	// Compute fade curves for each of x, y, z
	float u = fade(x);
	float v = fade(y);
	float w = fade(z);

	// Hash coordinates of the 8 cube corners
	int A = permutationTable[X] + Y;
	int AA = permutationTable[A] + Z;
	int AB = permutationTable[A + 1] + Z;
	int B = permutationTable[X + 1] + Y;
	int BA = permutationTable[B] + Z;
	int BB = permutationTable[B + 1] + Z;

	// Add blended results from 8 corners of cube
	float	res =	lerp(w, lerp(v, lerp(u, grad(permutationTable[AA], x, y, z), grad(permutationTable[BA], x - 1, y, z)),
									lerp(u, grad(permutationTable[AB], x, y - 1, z), grad(permutationTable[BB], x - 1, y - 1, z))),
							lerp(v, lerp(u, grad(permutationTable[AA + 1], x, y, z - 1), grad(permutationTable[BA + 1], x - 1, y, z - 1)),
									lerp(u, grad(permutationTable[AB + 1], x, y - 1, z - 1), grad(permutationTable[BB + 1], x - 1, y - 1, z - 1))));
    // linearly interpolate the nx0/nx1 along they y axis
	res = (res + 1.0) / 2.0;
    uvec3 noise = uvec3(res * 255);
	imageStore(resultImg, p.xy, uvec4(noise, 255));
}