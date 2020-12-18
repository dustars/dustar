#version 450 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (binding = 0, rgba8ui) uniform uimage2D img;

/* Available build-in variables for compute shader
gl_WorkGroupID
gl_LocalInvocationID
gl_NumWorkGroups
gl_WorkGroupSize
gl_GlobalInvocationID
gl_LocalInvocationIndex
*/

void main(void) {

	uvec4 texel;
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	//read data
	texel = imageLoad(img, p);
	//Don't inverse the alpha channel, otherwise the result will be transparent.
	texel.rgb = uvec3(255u) - texel.rgb;
	//write data
	imageStore(img, p, texel);
}