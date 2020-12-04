#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//Is there any problems with the binding points?
layout(location = 0) uniform uint maxTableSize = 256; //Currently hardcoded
layout(location = 1) uniform float samplingRate;

layout(binding = 0) buffer randomValueBlock {
	float randomValues[256];
};

layout(binding = 1) buffer permutationTableBlock {
	uint permutationTable[512];
};

layout (binding = 0, rgba8ui) uniform uimage2D resultImg;

float smoothstep(float t) {
    return t * t * (3 - 2 * t);
}

float lerp(float lo, float hi, float t){
    return lo * (1 - t) + hi * t;
}

void main(void) {

	ivec2 p = ivec2(gl_GlobalInvocationID.xy);

    vec2 p1 = p * samplingRate;
    const int kMaxTableSizeMask = int(maxTableSize) - 1;

    int xi = int(p1.x);
    int yi = int(p1.y);

    float tx = p1.x - xi;
    float ty = p1.y - yi;

    int rx0 = xi & kMaxTableSizeMask;
    int rx1 = (rx0 + 1) & kMaxTableSizeMask;
    int ry0 = yi & kMaxTableSizeMask;
    int ry1 = (ry0 + 1) & kMaxTableSizeMask;

    // random values at the corners of the cell using permutation table
    const float c00 = randomValues[permutationTable[permutationTable[rx0] + ry0]];
    const float c10 = randomValues[permutationTable[permutationTable[rx1] + ry0]];
    const float c01 = randomValues[permutationTable[permutationTable[rx0] + ry1]];
    const float c11 = randomValues[permutationTable[permutationTable[rx1] + ry1]];

    // remapping of tx and ty using the Smoothstep function 
    float sx = smoothstep(tx);
    float sy = smoothstep(ty);

    // linearly interpolate values along the x axis
    float nx0 = lerp(c00, c10, sx);
    float nx1 = lerp(c01, c11, sx);

    // linearly interpolate the nx0/nx1 along they y axis
    uvec3 noise = uvec3(lerp(nx0, nx1, sy) * 255);
	imageStore(resultImg, p, uvec4(noise, 255));
}