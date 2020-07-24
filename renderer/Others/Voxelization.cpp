#include "Voxelization.h"
#include "math/Math.h"

Voxelization::Voxelization()
{
	//Generate all textures needed and set up shader
}

Voxelization::~Voxelization()
{
	//Delete all textures
}

void Voxelization::CreateDepthTextures()
{
	glCreateFramebuffers(6, &frameBuffer[0]);
	glGenTextures(6, &depthTex[0]);
	for (std::size_t i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, depthTex[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 512, 512);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex[i], 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glGenTextures(1, &voxel3DTexture);
	glBindTexture(GL_TEXTURE_3D, voxel3DTexture);
	glTextureStorage3D(voxel3DTexture, 1, GL_R8UI, 512, 512, 2); //one channel, unsigned integer
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindImageTexture(0, voxel3DTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);


}

void Voxelization::StartVoxelization()
{
	//for (int i = 0; i < 6; i++) {
	//	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[i]);

	//	switch (i)
	//	{
	//	case 0:
	//		glClearDepth(1.0);
	//		glDepthFunc(GL_LESS);
	//		camera->SetPosition(Vector3(0.f, 0.f, 20.f)); camera->SetYaw(0.0f); camera->SetPitch(0.f);
	//		break;
	//	case 1:
	//		glClearDepth(0.0);
	//		glDepthFunc(GL_GREATER);
	//		camera->SetPosition(Vector3(0.f, 0.f, -20.f)); camera->SetYaw(180.0f); camera->SetPitch(0.f);
	//		break;
	//	case 2:
	//		glClearDepth(1.0);
	//		glDepthFunc(GL_LESS);
	//		camera->SetPosition(Vector3(20.f, 0.f, 0.f)); camera->SetYaw(90.0f); camera->SetPitch(0.f);
	//		break;
	//	case 3:
	//		glClearDepth(0.0);
	//		glDepthFunc(GL_GREATER);
	//		camera->SetPosition(Vector3(-20.f, 0.f, 0.f)); camera->SetYaw(270.0f); camera->SetPitch(0.f);
	//		break;
	//	case 4:
	//		glClearDepth(1.0);
	//		glDepthFunc(GL_LESS);
	//		camera->SetPosition(Vector3(0.f, 20.f, 0.f)); camera->SetPitch(-90.f); camera->SetYaw(0.f);
	//		break;
	//	case 5:
	//		glClearDepth(0.0);
	//		glDepthFunc(GL_GREATER);
	//		camera->SetPosition(Vector3(0.f, -20.f, 0.f)); camera->SetPitch(90.0f); camera->SetYaw(0.f);
	//		break;
	//	}

	//	TestRendering();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}
	//::SwapBuffers(deviceContext);

	////Generate voxel

	////Bind texture, need to be improved
	//glBindTextures(0, 6, depthTex);
	//glUseProgram(voxelShader.GetShader()->GetProgram());
	//glUniform1i(glGetUniformLocation(voxelShader.GetShader()->GetProgram(), "size"), 512);
	//voxelShader.Draw();
	//glUseProgram(0);
	//::SwapBuffers(deviceContext);
}
