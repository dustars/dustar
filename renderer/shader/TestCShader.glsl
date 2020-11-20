#version 450 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (binding = 0, rgba8) uniform image2D img;

/* Available build-in variables for compute shader
gl_WorkGroupID
gl_LocalInvocationID
gl_NumWorkGroups
gl_WorkGroupSize
gl_GlobalInvocationID
gl_LocalInvocationIndex
*/

void main(void) {

	vec4 texel;
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	//read data
	texel = imageLoad(img, p);
	//Make change
	texel = vec4(1.0) - texel;
	//write data
	imageStore(img, p, texel);
}