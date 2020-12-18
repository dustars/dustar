#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//Is there any problems with the binding points?
layout(location = 0) uniform uint maxTableSize = 256; //Currently hardcoded
layout(location = 1) uniform float samplingRate;
layout(location = 2) uniform int layers;
layout(location = 3) uniform float lacunarity;
layout(location = 4) uniform float gain;

layout(binding = 0) buffer permutationTableBlock {
	int permutationTable[512];
};

layout(binding = 1) buffer outputBlock {
	float noiseBuffer[512 * 512];
};

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

float noise(vec3 p) {

	float x = p.x;
	float y = p.y;
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
	float res =	lerp(w, lerp(v, lerp(u, grad(permutationTable[AA], x, y, z), grad(permutationTable[BA], x - 1, y, z)),
								lerp(u, grad(permutationTable[AB], x, y - 1, z), grad(permutationTable[BB], x - 1, y - 1, z))),
						lerp(v, lerp(u, grad(permutationTable[AA + 1], x, y, z - 1), grad(permutationTable[BA + 1], x - 1, y, z - 1)),
								lerp(u, grad(permutationTable[AB + 1], x, y - 1, z - 1), grad(permutationTable[BB + 1], x - 1, y - 1, z - 1))));

	return res = (res + 1.0) / 2.0;
}

void main(void) {

	vec3 p = vec3(gl_GlobalInvocationID.xyz) * samplingRate;

	float n = 0.f;
	float frequency = 1.f;
	float amplitude = 1.f;
	float maxValue = 0.f;

	for (int i = 0; i < layers; i++) {
		n += noise(p * frequency) * amplitude;
		maxValue += amplitude;
		frequency *= lacunarity;
		amplitude *= gain;
	}

	noiseBuffer[gl_GlobalInvocationID.x * 512 + gl_GlobalInvocationID.y] = n/maxValue;
}