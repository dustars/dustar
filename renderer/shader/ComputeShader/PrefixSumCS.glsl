#version 450 core

/* Available build-in variables for compute shader
//Within a single work group
gl_LocalInvocationID
gl_WorkGroupSize
//Withnin the whole domain
gl_WorkGroupID
gl_NumWorkGroups
//Thread on a global scale
gl_GlobalInvocationID		//xyz form
gl_LocalInvocationIndex		//index form
*/

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0) coherent buffer inputBlock {
	float input_data[gl_WorkGroupSize.x];
};

layout(binding = 1) coherent buffer outputBlock {
	float output_data[gl_WorkGroupSize.x];
};

//shared variable is accessed much faster than images or storage blocks, but what its' maximum size?
//这算是一种特殊的，适用于GPU架构的结构吗？？local variable有register for each SMID unit,这玩意儿大概率在LDS中（GCN架构）
//而这算是GPU的一种特殊硬件？
//数据在GPU中的中转地……输入进来，处理，然后输出给output，一定要明确其定位。
//查了一下，一共可用49152 btyes，50KB的大小还不错了。
shared float shared_data[gl_WorkGroupSize.x * 2];

void main(void) {
	uint id = gl_LocalInvocationID.x;
	uint rd_id;
	uint wr_id;
	uint mask;
	// The number of steps is the log base 2 of the
	// work group size, which should be a power of 2
	const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1;
	uint step = 0;
	// Each invocation is responsible for the content of
	// two elements of the output array
	shared_data[id * 2] = input_data[id * 2];
	shared_data[id * 2 + 1] = input_data[id * 2 + 1];
	// Synchronize to make sure that everyone has initialized
	// their elements of shared_data[] with data loaded from
	// the input arrays
	barrier();
	memoryBarrierShared();
	// For each step...
	for (step = 0; step < steps; step++)
	{
		// Calculate the read and write index in the
		// shared array
		mask = (1 << step) - 1; //Those bitwise operations are killing me
		rd_id = ((id >> step) << (step + 1)) + mask;
		wr_id = rd_id + 1 + (id & mask);
		// Accumulate the read data into our element
		shared_data[wr_id] += shared_data[rd_id];
		// Synchronize again to make sure that everyone
		// has caught up with us
		barrier();
		memoryBarrierShared();
	}

	// Finally write our data back to the output image
	output_data[id * 2] = shared_data[id * 2];
	output_data[id * 2 + 1] = shared_data[id * 2 + 1];
}