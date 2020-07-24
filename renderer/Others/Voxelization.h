/*
	Description:
	The implementation of voxelization algorithm
	original paper: https://www.researchgate.net/publication/215506131_A_Fast_Depth-Buffer-Based_Voxelization_Algorithm

	This class is unfinished.
	
	Created: 7/23/2020
	Last Updated:7/23/2020

	Todo:
	It needs a specific rendering pipeline to first draw 6 sides of the object
	into 6 depth buffers, and then pass them into the voxelization shader to
	generate the 3D voxel texture (see paper for reference).
	To implement, take RenderObject class as an input parameter, and then use
	shader defined here to do the depth rendering (not implemented yet).
*/

#pragma once
#include <GL/glew.h>

class Voxelization
{
public:
	Voxelization();
	~Voxelization();

	GLuint GetVoxelTexture() { return voxel3DTexture; }

private:
	//6 depth buffers and their fbo.
	GLuint depthTex[6];
	GLuint frameBuffer[6];

	//The generated 3D voxel data.
	GLuint voxel3DTexture;

	void CreateDepthTextures();
	void StartVoxelization();
};

