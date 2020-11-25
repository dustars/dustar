//TODO: format, why P is inverse?

#version 450 core

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

shared uvec3 shared_data[gl_WorkGroupSize.x * 2];

layout(binding = 0, rgba8ui) uniform uimage2D input_image;

void main(void)
{
	uint id = gl_LocalInvocationID.x;
	uint rd_id;
	uint wr_id;
	uint mask;

	const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1;
	uint step = 0;

	ivec2 P = ivec2(id * 2, gl_WorkGroupID.x);

	shared_data[id * 2]		= imageLoad(input_image, P).rgb;
	shared_data[id * 2 + 1]	= imageLoad(input_image, P + ivec2(1, 0)).rgb;

	barrier();
	memoryBarrierShared();

	for (step = 0; step < steps; step++)
	{
		mask = (1 << step) - 1;
		rd_id = ((id >> step) << (step + 1)) + mask;
		wr_id = rd_id + 1 + (id & mask);

		shared_data[wr_id] += shared_data[rd_id];

		barrier();
		memoryBarrierShared();
	}

	imageStore(input_image, P.yx, uvec4(shared_data[id * 2], 255));
	imageStore(input_image, P.yx + ivec2(0, 1), uvec4(shared_data[id * 2 + 1], 255));
}