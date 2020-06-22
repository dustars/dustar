#include "Renderer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdParty/stb_image_write.h"

Renderer::Renderer(Window& parent) : RenderBase(parent)
{
#ifdef TESTING
	Testing();
#else
	// Initialize the basics ( later move to a function )
	camera = new Camera(-20,-130,Vector3(0,500,200.f));
	//camera = new Camera(0,0,Vector3(0,0,0));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	// Map
	object = new RenderObject();
	if (!object->SetShader("shader/PhongShadingVS.glsl", "shader/PhongShadingFS.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	if (!object->GetTexture()->SetTexture("../assets/Textures/Barren Reds.jpg")) {
		cout << "Texture set up failed!" << endl;
	}

	// Lightings
	pointLight1 = new PointLight(Vector4(2000.f, 1000.f, 2000.f ,1.f), Vector4(0.9f, 0.8f, 0.4f, 1.f));

	//octave, lacunarity, persistence, width, length
	object->SetMesh(new HeightMap(5, 3, 0.4, MAPWIDTH, MAPLENGTH));

	//Particle System Creation
	//ParticleSystem::renderer = this; // Let all particle systems be able to access the resources
	//particleMaster = new ParticleMaster();
	//particleMaster->AddSystem(new ParticleSystem(60, { 350,100,350 }, 5.f, 1, 100));
	//particleMaster->AddSystem(new ParticleSystem(10, { 300,100,300 }, 5.f, 0, 25, EmitType::TRAJECTORY));

	CreateSkybox();
	//CreateTrajectory();

	init = true;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#endif
}

Renderer::~Renderer() {
	delete camera;
	if (object) {
		delete object;
	}
	if (trajectory) {
		delete trajectory;
	}
	if (skybox) {
		delete skybox;
	}
	if (pointLight1) {
		delete pointLight1;
	}
	if (particleMaster) {
		delete particleMaster;
	}
}

void Renderer::Update(float dt) {

	// Todo: Put into the 60 fps loop
	camera->UpdateCamera(dt);

	// Temporary method to limit frame rate at 60FPS
	oneFrame += dt;
	if (oneFrame > renderFrames) {
		////////////////////////
		//  Locations Update  //
		////////////////////////
		if (particleMaster) {
			particleMaster->Update(oneFrame);
		}

		if (trajectory) {
			trajectory->GetMesh()->Update(oneFrame);
		}

		////////////////////////
		// Graphics Rendering //
		////////////////////////
#ifdef TESTING
		//RealTimeVoxelization();
		TestRendering();
#else
		Render();
#endif
		oneFrame = 0;
	}
}

void Renderer::Render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (skybox) {
		renderSkyBox();
	}
	
	if (object) {
		renderObject();
	}

	if (trajectory) {
		glUseProgram(trajectory->GetShader()->GetProgram());
		glUniformMatrix4fv(glGetUniformLocation(trajectory->GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&camera->BuildViewMatrix());
		glUniformMatrix4fv(glGetUniformLocation(trajectory->GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
		glUniformMatrix4fv(glGetUniformLocation(trajectory->GetShader()->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
		trajectory->Draw();
		glUseProgram(0);
	}

	if (particleMaster) {
		particleMaster->Render();
	}

	::SwapBuffers(deviceContext);
}

void Renderer::renderObject()
{
	glUseProgram(object->GetShader()->GetProgram());
	glBindTextureUnit(3, worleyTex);
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&camera->BuildViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	if (pointLight1) {
		glUniform3fv(glGetUniformLocation(object->GetShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
		//pointLight1->SetPosition(Vector4(camera->GetPosition(), 1.0));
		glUniform4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "LightPos"), 1, (float*)&pointLight1->GetPosition());
		glUniform4fv(glGetUniformLocation(object->GetShader()->GetProgram(), "LightColor"), 1, (float*)&pointLight1->GetColor());
	}
	object->Draw();
	glUseProgram(0);
}

void Renderer::renderSkyBox()
{
	glUseProgram(skybox->GetShader()->GetProgram());
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glUniformMatrix4fv(glGetUniformLocation(skybox->GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE,(float*)&camera->BuildViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(skybox->GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetTexture()->GetTexture());
	
	skybox->GetMesh()->Draw();

	glDepthMask(GL_TRUE);
	glUseProgram(0);
}

void Renderer::RealTimeVoxelization()
{
	for (int i = 0; i < 6; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[i]);

		switch (i)
		{
		case 0:
			glClearDepth(1.0);
			glDepthFunc(GL_LESS);
			camera->SetPosition(Vector3(0.f, 0.f, 20.f)); camera->SetYaw(0.0f); camera->SetPitch(0.f);
			break;
		case 1:
			glClearDepth(0.0);
			glDepthFunc(GL_GREATER);
			camera->SetPosition(Vector3(0.f, 0.f, -20.f)); camera->SetYaw(180.0f); camera->SetPitch(0.f);
			break;
		case 2:
			glClearDepth(1.0);
			glDepthFunc(GL_LESS);
			camera->SetPosition(Vector3(20.f, 0.f, 0.f)); camera->SetYaw(90.0f); camera->SetPitch(0.f);
			break;
		case 3:
			glClearDepth(0.0);
			glDepthFunc(GL_GREATER);
			camera->SetPosition(Vector3(-20.f, 0.f, 0.f)); camera->SetYaw(270.0f); camera->SetPitch(0.f);
			break;
		case 4:
			glClearDepth(1.0);
			glDepthFunc(GL_LESS);
			camera->SetPosition(Vector3(0.f, 20.f, 0.f)); camera->SetPitch(-90.f); camera->SetYaw(0.f);
			break;
		case 5:
			glClearDepth(0.0);
			glDepthFunc(GL_GREATER);
			camera->SetPosition(Vector3(0.f, -20.f, 0.f)); camera->SetPitch(90.0f); camera->SetYaw(0.f);
			break;
		}

		TestRendering();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	::SwapBuffers(deviceContext);

	//Generate voxel

	//Bind texture, need to be improved
	glBindTextures(0, 6, depthTex);
	glUseProgram(voxelShader.GetShader()->GetProgram());
	glUniform1i(glGetUniformLocation(voxelShader.GetShader()->GetProgram(), "size"), 512);
	voxelShader.Draw();
	glUseProgram(0);
	::SwapBuffers(deviceContext);
}

void Renderer::CreateCloud()
{
	if (!cloudShader.SetShader("shader/CloudVShader.glsl", "shader/CloudFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	cloudShader.GetMesh()->CreatePlane(); //ray marching plane

	CreateCloud3DTexture();
}

void Renderer::RenderCloud()
{
	glUseProgram(cloudShader.GetShader()->GetProgram());

	glBindTextureUnit(0, renderFBO->GetColorTexture());
	glBindTextureUnit(1, renderFBO->GetDepthTexture());

	glUniform2fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "resolution"), 1, (float*)&Vector2(width, height));
	glUniform3fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());;
	Matrix4 rotationMatrix = Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(camera->GetPitch(), Vector3(1, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "rotationMatrix"), 1, GL_FALSE, (float*)&rotationMatrix);

	cloudShader.Draw();
	glUseProgram(0);
}

void Renderer::CreateCloud3DTexture()
{
	int width = 500, height = 500, length = 128; // This resolution is from the GPU PRO
	int octaves = 4; //FBM
	float lacunarity = 2;
	float gain = 2;
	int MAX_POINTS = 20;
	unsigned char* data = new unsigned char[width * height];

	vector<Vector2> featurePoints;

	//Generate randomly distributed points.
	srand(time(NULL));
	for (int i = 0; i < MAX_POINTS; i++) {
		featurePoints.push_back(Vector2(static_cast<float>(rand())/(RAND_MAX), static_cast<float>(rand()) / (RAND_MAX)));
	}

	//lambda to calculate the n-th closest point, given a location.
	auto dist = [&](Vector2 point) {
		float min = 1.f;
		for (int i = 0; i < featurePoints.size(); i++) {
			auto temp = sqrt(pow((point.x - featurePoints[i].x), 2) + pow((point.y - featurePoints[i].y), 2));
			if (temp < min) min = temp;
		}
		//Currently return inverted distance value. If you want orginal one, delete "1 - ".
		return 1 - Clamp(Remap(min, 0.f, 0.3f, 0.f, 1.f), 0.f, 1.f);
	};	

	//for(int i = 0; i < octaves; i++){
	//calculate the distance of the closest feature point to the current pixel.
		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				int color = dist(Vector2(static_cast<float>(x) / height, static_cast<float>(y) / width)) * 255;
				data[x * width + y] = static_cast<unsigned char>(color);
			}
		}
	//}

	glGenTextures(1, &worleyTex);
	glBindTexture(GL_TEXTURE_2D, worleyTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<void*>(data));
	//SaveAsImage(width, height, static_cast<void*>(data), 1);

	delete[] data;
}

void Renderer::CreatePerlinWorleyNoise()
{
}

void Renderer::CreateSkybox()
{
	skybox = new RenderObject();
	if (!skybox->SetShader("shader/SkyBoxVShader.glsl", "shader/SkyBoxFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	skybox->GetTexture()->CreateCubeMap("../assets/Skybox/bluecloud_rt.jpg",
		"../assets/Skybox/bluecloud_lf.jpg",
		"../assets/Skybox/bluecloud_up.jpg",
		"../assets/Skybox/bluecloud_dn.jpg",
		"../assets/Skybox/bluecloud_bk.jpg",
		"../assets/Skybox/bluecloud_ft.jpg");
	skybox->GetMesh()->CreateQuad();
}

void Renderer::CreateTrajectory()
{
	trajectory = new RenderObject();

	if (!trajectory->SetShader("shader/TriangleVShader.glsl", "shader/TriangleFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	trajectory->SetMesh(new Trajectory());
}

void Renderer::CreateVoxelizationResources()
{
	glCreateFramebuffers(6, &frameBuffer[0]);
	glGenTextures(6, &depthTex[0]);
	for (size_t i = 0; i < 6; i++)
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
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(voxel3DTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindImageTexture(0, voxel3DTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);

	voxelShader.SetShader("shader/VoxelizationVShader.glsl", "shader/VoxelizationFShader.glsl");
	voxelShader.GetMesh()->CreateCube();
}

void Renderer::TestRendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	if (skybox) {
		renderSkyBox();
	}
	
	glEnable(GL_DEPTH_TEST);
	if (object) {
		renderObject();
	}
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderCloud();

	::SwapBuffers(deviceContext);
}

void Renderer::ScreenShot()
{
	int row_size = ((width * 3 + 3) & ~3);
	int data_size = row_size * height;
	unsigned char* data = new unsigned char[data_size];

	glReadPixels(0,0,width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	stbi_flip_vertically_on_write(TRUE);
	stbi_write_jpg("../demo/screenshot.jpg", width, height, 3, data, 100);

	delete[] data;
}

void Renderer::Voxelization(int size)
{
	// 1. Do we get depth value correctly (method)
	// 2. Do we have all 6 depth textures correct (data)
	// 3. Do we have correct format for textures, both Zbuffer and voxel?
	// 4. Do we generate voxel data correctly?
	// 5. How can I visualize the voxel?

	GLuint depthTex[6];
	GLuint frameBuffer;

	glGenTextures(6, depthTex);
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, depthTex[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, size, size);
	}

	glCreateFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	for (int i = 0; i < 6; i++) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex[i], 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		renderObject();
		// Rotate camera
		::SwapBuffers(deviceContext);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (!object->SetShader("shader/VoxelizationVShader.glsl", "shader/VoxelizationFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	//Bind texture, need to be improved
	glBindTexture(GL_TEXTURE_2D, depthTex[0]);

	glUseProgram(object->GetShader()->GetProgram());
	glUniform1i(glGetUniformLocation(object->GetShader()->GetProgram(), "size"), size);
	object->Draw();
	glUseProgram(0);
}

void Renderer::SaveAsImage(int w, int h, const void* data, int channels, int quality)
{
	stbi_write_jpg("../demo/test.jpg", w, h, channels, data, quality);
}

void Renderer::Testing()
{
	camera = new Camera(0.f, 0.f, Vector3(0.f, 0.f, -10.f));

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	object = new RenderObject();
	if (!object->SetShader("shader/TextureOnlyVShader.glsl", "shader/TextureOnlyFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	if (!object->GetTexture()->SetTexture("../assets/Textures/container.jpg")) {
		cout << "Texture set up failed!" << endl;
	}

	modelMatrix = modelMatrix * Matrix4::Scale(Vector3(10.f, 10.f, 10.f));

	pointLight1 = new PointLight(Vector4(0.f, 0.f, 500.f, 1.f), Vector4(0.9f, 0.8f, 0.4f, 1.f));

	object->GetMesh()->CreatePlane();

	renderFBO.reset(new GeneralFrameBuffer(width, height));

	CreateSkybox();
	CreateCloud();

	init = true;
	glEnable(GL_DEPTH_TEST);
}

