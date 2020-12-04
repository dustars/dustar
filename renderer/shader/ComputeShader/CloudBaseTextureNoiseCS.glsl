#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Hard-coded parameters
const int resolution = 128;
const int numOfCells = 8;
const float samplingRate = 1.f;
// For FBM
const float lacunarity = 2.f;
const float gain = 0.707f;

layout(binding = 0) buffer permutationTableBlock {
	int permutationTable[512];
};

layout(binding = 1, std430) buffer featurePointsBlock {
	vec3 featurePoints[numOfCells * numOfCells * numOfCells];
};

layout (binding = 0, rgba32f) uniform image3D resultImg;

float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
	return a + t * (b - a);
}

float grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float EulerDistance(float x, float y, float z, float x1, float y1, float z1) {
	float disX = x1 - x;
	float disY = y1 - y;
	float disZ = z1 - z;
	disX *= disX;
	disY *= disY;
	disZ *= disZ;
	disX += disY + disZ;
	return sqrt(disX);
}

float WorleyNoise(vec3 p){
	float x = p.x;
	float y = p.y;
	float z = p.z;

	//Relative position of point in a cube
	x /= float(resolution / numOfCells);
	y /= float(resolution / numOfCells);
	z /= float(resolution / numOfCells);

	//which cube the point is in, mod the number of cells to ensure periodicity
	int cX = int(x) & (numOfCells - 1);
	int cY = int(y) & (numOfCells - 1);
	int cZ = int(z) & (numOfCells - 1);

	//Relative position of a point, from 0 to 1.
	x -= int(x);
	y -= int(y);
	z -= int(z);

	//Calculating distances in neighboring cubes, 3 x 3 x 3  = 27 in totals.
	float min = 2.f;
	int kk = 0, jj = 0, ii = 0;
	for (int i = -1; i <= 1; i++) {
		if (cZ + i == -1)			ii = numOfCells;
		if (cZ + i == numOfCells)	ii = 0 - numOfCells;
		for (int j = -1; j <= 1; j++) {
			if (cY + j == -1)			jj = numOfCells;
			if (cY + j == numOfCells)	jj = 0 - numOfCells;
			for (int k = -1; k <= 1; k++) {
				if (cX + k == -1)			kk = numOfCells;
				if (cX + k == numOfCells)	kk = 0 - numOfCells;
				vec3 fp = featurePoints[(cZ + i + ii) * numOfCells * numOfCells + (cY + j + jj) * numOfCells + (cX + k + kk)];
				float temp = EulerDistance(x, y, z, fp.x + k, fp.y + j, fp.z + i);
				if (temp < min) min = temp;
				kk = 0;
			}
			jj = 0;
		}
		ii = 0;
	}

	return min;
}

float PerlinNoise(vec3 p) {

	float x = p.x;
	float y = p.y;
	float z = p.z;

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
	float res = lerp(w, lerp(v, lerp(u, grad(permutationTable[AA], x, y, z), grad(permutationTable[BA], x - 1, y, z)),
								lerp(u, grad(permutationTable[AB], x, y - 1, z), grad(permutationTable[BB], x - 1, y - 1, z))),
						lerp(v, lerp(u, grad(permutationTable[AA + 1], x, y, z - 1), grad(permutationTable[BA + 1], x - 1, y, z - 1)),
								lerp(u, grad(permutationTable[AB + 1], x, y - 1, z - 1), grad(permutationTable[BB + 1], x - 1, y - 1, z - 1))));

	return res = (res + 1.0) / 2.0;
}

float PerlinWorleyNoise(vec3 p){
	// FBM Worley Noise
	float worleyNoise = 0.f;
	int layers = 3;
	float frequency = 1.f;
	float amplitude = 1.f;
	float maxValue = 0.f;

	for (int i = 0; i < layers; i++) {
		worleyNoise += WorleyNoise(p * frequency) * amplitude;
		maxValue += amplitude;
		frequency *= lacunarity;
		amplitude *= gain;
	}
	worleyNoise /= maxValue;
	worleyNoise = 1 - worleyNoise;

	// FBM Perlin Noise
	float perlinNoise = 0.f;
	layers = 7;
	frequency = 1.f;
	amplitude = 1.f;
	maxValue = 0.f;

	p = p / resolution * 3.0f; //?

	for (int i = 0; i < layers; i++) {
		perlinNoise += PerlinNoise(p * frequency) * amplitude;
		maxValue += amplitude;
		frequency *= lacunarity;
		amplitude *= gain;
	}
	perlinNoise /= maxValue;

	worleyNoise = clamp(map(worleyNoise, 0.1f, 0.9f, 0.f, 1.f), 0.f, 1.f);
	perlinNoise = clamp(map(perlinNoise, 0.3f, 0.7f, 0.f, 1.1f), 0.f, 1.f);

	return clamp(map(perlinNoise * worleyNoise, 0.f, 1.f, 0.2f, 1.7f), 0.f, 1.f);
}

void main(void) {
	vec3 p = vec3(gl_GlobalInvocationID.xyz) * samplingRate; //?
	vec4 noise = vec4(PerlinWorleyNoise(p),
					  1 - WorleyNoise(p * 0.5),
					  1 - WorleyNoise(p),
					  1 - WorleyNoise(p * 1.5) );
	imageStore(resultImg, ivec3(gl_GlobalInvocationID.xyz), noise);
}