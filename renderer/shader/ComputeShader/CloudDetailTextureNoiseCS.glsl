#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//Hard-coded parameters
const int resolution = 32;
const int numOfCells = 4;
const float samplingRate = 1.f;

layout(binding = 0, std430) buffer featurePointsBlock {
	vec3 featurePoints[numOfCells * numOfCells * numOfCells];
};

layout (binding = 0, rgba32f) uniform image3D resultImg;

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

float WorleyNoise(float x, float y, float z){
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

void main(void) {
	vec3 p = vec3(gl_GlobalInvocationID.xyz) * samplingRate;
	float x = p.x;
	float y = p.y;
	float z = p.z;
	vec3 noise = vec3(WorleyNoise(x * 0.5, y * 0.5, z * 0.5),
					  WorleyNoise(x, y, z),
					  WorleyNoise(x * 1.5, y * 1.5, z * 1.5) );
	imageStore(resultImg, ivec3(gl_GlobalInvocationID.xyz), vec4(1 - noise, 1.f));
}