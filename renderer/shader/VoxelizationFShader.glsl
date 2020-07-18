#version 450 core

uniform sampler2D ZFront;
uniform sampler2D ZBack;
uniform sampler2D XFront;
uniform sampler2D XBack;
uniform sampler2D YFront;
uniform sampler2D YBack;

uniform int size;
layout (binding = 0) writeonly uniform uimage3D voxelImage;

out vec4 FragColor;

void main(void)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			//for (int k = 0; k < size; k++)
			// K is now 2, not 512
			//{
			//	imageStore(voxelImage, ivec3(0, j, k), uvec4(1, 1, 0, 1));
			//}


			texelFetch(XFront, ivec2(i, j), 0) // The type of return value is 32-bit floating point


			imageStore(voxelImage, ivec3(i, j, 0), uvec4(1, 0, 0, 1)); // Only the first channel of the data will be stored
		}
	}
	discard;
}