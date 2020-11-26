//TODO: format, why P is inverse?

#version 450 core

layout(local_size_x = 512, local_size_y = 1, local_size_z = 1) in;

shared vec3 shared_data[gl_WorkGroupSize.x * 2];

layout (binding = 0, rgba32f) readonly uniform image2D input_image;
layout (binding = 1, rgba32f) writeonly uniform image2D output_image;

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

	// Is 1.f here valid?
	imageStore(output_image, P.yx, vec4(shared_data[id * 2], 1.f));
	imageStore(output_image, P.yx + ivec2(0, 1), vec4(shared_data[id * 2 + 1], 1.f));
}