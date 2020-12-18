#include "Renderer.h"
#include <stb_image.h>
#include <stb_image_write.h>

Renderer::Renderer(Window& parent)
	: RenderBase(parent),
	light1(new PointLight(Vector3(2000.f, 600.f, 2000.f), Vector4(1.0f, 0.9f, 0.9f, 1.f))),
	renderFBO(new FrameBuffer(width, height)),
	SATComputeShader("shader/ComputeShader/PrefixSum2DCS.glsl"),
	textRenderer(TextRenderer(width, height)),
	cloudCS("shader/ComputeShader/CloudRenderingCS.glsl")
{
	//ComputeShaderPlayground();

	//camera = new Camera(parent, -20.f, 40.0f, Vector3(2000.f, 1000.f, 2000.f));
	camera = new Camera(parent, 0, 0, Vector3(0.f, 0.f, 0.f));
	projMatrix = Matrix4::Perspective(1.0f, 20000.0f, (float)width / (float)height, 45.0f);

	//Testing
	//SetTransformUBO();
	//CreateAtomicBuffer();

	CreateObject();
#ifdef MODEL_OBJECT
	CreateModelObject();
#endif // MODEL_OBJECT

#ifdef ATMOSPHERE
	CreateAtmosphericScatteringModel();
#else
	CreateSkybox();
#endif // ATMOSPHERE

#ifdef RENDER_CLOUD
#ifdef RENDER_CLOUD_CS
	CreateCloudCS();
#else
	CreateCloud();
#endif // Compute Shader
#endif // RENDER_CLOUD

#ifdef SHADOW_MAPPING
	CreateShadowMap();
#endif // SHADOW_MAPPING

#ifdef POST_PROCESSING
	postProcessingFBO.reset(new FrameBuffer(width, height));

#ifdef DEPTH_OF_FIELD
	CreateDepthOfField();
#endif// DEPTH_OF_FIELD

#endif //POST_PROCESSING

#ifdef IMGUI
	ImGUIInit(parent);
#endif // IMGUI
	init = true;
}

Renderer::~Renderer()
{
	if (object)						delete object;
	if (camera)						delete camera;
	if (trajectory)					delete trajectory;
	if (skybox)						delete skybox;
	if (particleMaster)				delete particleMaster;
#ifdef IMGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void Renderer::Update(float dt)
{
	camera->UpdateCamera(dt);
	UpdateControl(dt);

	//Temporary method to limit frame rate at 60FPS
	oneFramePerMilliSecond += dt;
	if (oneFramePerMilliSecond > renderFrames) {
		//Update
		if (particleMaster) particleMaster->Update(oneFramePerMilliSecond);
		//if (trajectory) trajectory->GetMesh()->Update(oneFramePerMilliSecond);
		UtilityUpdate();
#ifdef RENDER_CLOUD
		//cloudModel->Update(dt);
#endif
		//Render
		Render();
	}
}

void Renderer::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef SHADOW_MAPPING
	RenderShadowMap(); //Is it better to use "create" instead of "render"?
#endif

#ifdef RENDER_CLOUD
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#else
#ifdef POST_PROCESSING
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#endif // POST_PROCESSING
#endif

	//--------------------------
	//---Background Rendering---
	//--------------------------
#ifdef ATMOSPHERE
	if (atmosphereScattering.get()) RenderAtmosphericScatteringModel();
#else
	if (skybox) RenderSkyBox();
#endif // ATMOSPHERE

#ifdef RENDER_CLOUD
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
#ifdef POST_PROCESSING
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#endif // POST_PROCESSING
#endif // RENDER_CLOUD

	//---------------------
	//---Cloud Rendering---
	//---------------------
#ifdef RENDER_CLOUD
#ifdef RENDER_CLOUD_CS
	RenderCloudCS();
#else
	RenderCloud();
#endif // Compute Shader
#endif

	//-----------------------
	//---Objects Rendering---
	//-----------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

#ifdef MODEL_OBJECT
	RenderModelObject();
#endif // MODEL_OBJECT

#ifdef SQUARE_OBJECT
	//RenderObject();
#else
	RenderObject();
#endif // SQUARE_OBJECT
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//--------------------------------------
	//---Post Processing Effect Rendering---
	//--------------------------------------
#ifdef POST_PROCESSING
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef DEPTH_OF_FIELD
	RenderDepthOfField();
#endif //DEPTH_OF_FIELD

#endif //POST_PROCESSING

#ifdef OFFLINE
	ScreenShot("Offline_Rendering");
#else
	UtilityRender();
	SwapBuffers(deviceContext);
#endif
}

void Renderer::UtilityUpdate()
{
	oneFramePerSecond += oneFramePerMilliSecond;
	frameCount++;
	oneFramePerMilliSecond = 0;
	if (oneFramePerSecond > 1000.f) {
		oneFramePerSecond = 0;
		fps = frameCount;
		frameCount = 0;
	}
}

void Renderer::UtilityRender()
{
#ifdef IMGUI
	if (camera->GetShowGUI()) RenderImGUI();
#endif // IMGUI
	if (isRenderingText) RenderText();
}

void Renderer::CreateObject()
{
	object = new GameObject();
#ifdef SQUARE_OBJECT
	if (!object->SetShader("shader/GeneralVS.glsl", "shader/GeneralFS.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	if (!object->SetTexture("../assets/Textures/container.jpg")) {
		cout << "Texture set up failed!" << endl;
	}
	object->SetMeshAsQuad();
#else
	if (!object->SetShader("shader/GeneralVS.glsl", "shader/GeneralFS.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	if (!object->SetTexture("../assets/Textures/Barren Reds.jpg")) {
		cout << "Texture set up failed!" << endl;
	}
	object->SetMesh(new HeightMap(5, 2, 0.707, MAPWIDTH, MAPLENGTH));
#endif
}

void Renderer::RenderObject()
{
	//ResetAtomicBuffer();
	glUseProgram(object->GetProgram());
	glBindTextureUnit(0, object->GetTexture());

#ifdef SHADOW_MAPPING
	glBindTextureUnit(2, shadowFBO->GetDepthTexture());
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "lightMatrix"), 1, GL_FALSE, (float*)&lightMatrix);
#endif
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	cameraMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&cameraMatrix);
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	if (light1.get()) {
		Vector3 tempPos = camera->GetPosition();
		glUniform3fv(glGetUniformLocation(object->GetProgram(), "cameraPos"), 1, (float*)&tempPos);
		Vector3 tempDir = camera->GetSunDirection();
		glUniform3fv(glGetUniformLocation(object->GetProgram(), "sunDir"), 1, (float*)&tempDir);
		tempPos = light1->GetPosition();
		glUniform4fv(glGetUniformLocation(object->GetProgram(), "LightPos"), 1, (float*)&tempPos);
		Vector4 tempColor = light1->GetColor();
		glUniform4fv(glGetUniformLocation(object->GetProgram(), "LightColor"), 1, (float*)&tempColor);
	}

	object->Draw();

	glBindTextureUnit(0, 0);
	glUseProgram(0);
}

void Renderer::CreateSkybox()
{
	skybox = new GameObject();
	if (!skybox->SetShader("shader/SkyBoxVShader.glsl", "shader/SkyBoxFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	skybox->SetTexture("../assets/Skybox/bluecloud_rt.jpg",
		"../assets/Skybox/bluecloud_lf.jpg",
		"../assets/Skybox/bluecloud_up.jpg",
		"../assets/Skybox/bluecloud_dn.jpg",
		"../assets/Skybox/bluecloud_bk.jpg",
		"../assets/Skybox/bluecloud_ft.jpg");
	skybox->SetMeshAsQuad();

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}


void Renderer::CreateModelObject()
{
	modelObject.reset(new AssimpModel("../assets/Models/backpack/backpack.obj"));
	//modelObject.reset(new AssimpModel("../assets/Models/Aya/091_W_Aya_100K.obj"));
	//modelObject.reset(new AssimpModel("../assets/Models/Standord_Bunny/bun_zipper.ply"));
}

void Renderer::RenderModelObject()
{
	//ResetAtomicBuffer();
	glUseProgram(object->GetProgram());

#ifdef SHADOW_MAPPING
	glBindTextureUnit(2, shadowFBO->GetDepthTexture());
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "lightMatrix"), 1, GL_FALSE, (float*)&lightMatrix);
#endif
	//Matrix4 temp = modelMatrix * Matrix4::Scale(Vector3(0.01, 0.01, 0.01));
	//glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&temp);
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	cameraMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&cameraMatrix);
	glUniformMatrix4fv(glGetUniformLocation(object->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	if (light1.get()) {
		Vector3 tempPos = camera->GetPosition();
		glUniform3fv(glGetUniformLocation(object->GetProgram(), "cameraPos"), 1, (float*)&tempPos);
		Vector3 tempDir = camera->GetSunDirection();
		glUniform3fv(glGetUniformLocation(object->GetProgram(), "sunDir"), 1, (float*)&tempDir);
		tempPos = light1->GetPosition();
		glUniform4fv(glGetUniformLocation(object->GetProgram(), "LightPos"), 1, (float*)&tempPos);
		Vector4 tempColor = light1->GetColor();
		glUniform4fv(glGetUniformLocation(object->GetProgram(), "LightColor"), 1, (float*)&tempColor);
	}

	modelObject->Draw();

	glUseProgram(0);
}

void Renderer::RenderSkyBox()
{
	glDepthMask(GL_FALSE);
	glUseProgram(skybox->GetProgram());
	glBindTextureUnit(0, skybox->GetTexture());
	cameraMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skybox->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&cameraMatrix);
	glUniformMatrix4fv(glGetUniformLocation(skybox->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);

	skybox->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::CreateCloud()
{
	if (!cloudShader.SetShader("shader/CloudVShader.glsl", "shader/CloudFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	cloudShader.SetMeshAsQuad(); //ray marching plane

	cloudModel.reset(new atmosphere::Cloud(128));

	glUseProgram(cloudShader.GetProgram());
	glBindTextureUnit(6, cloudModel->GetBaseShapeTex());
	glBindTextureUnit(7, cloudModel->GetDetailShapeNoiseTex());
	glBindTextureUnit(8, cloudModel->GetWeatherMapTex());
	glBindTextureUnit(9, cloudModel->GetBlueNoiseTex());

	glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetProgram(), "projMatrix"), 1, GL_FALSE, (float*)&projMatrix);

	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "cloudLayerRadius"), cloudModel->cloudLayerRadius);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "cloudHeightAboveGround"), cloudModel->cloudHeightAboveGround);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "cloudLayerLength"), cloudModel->cloudLayerLength);

	glUseProgram(0);
}

void Renderer::RenderCloud()
{
	glUseProgram(cloudShader.GetProgram());
	glBindTextureUnit(0, renderFBO->GetColorTexture());

	cameraMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetProgram(), "viewMatrix"), 1, true, (float*)&cameraMatrix);
	Vector3 tempPos = camera->GetPosition();
	glUniform3fv(glGetUniformLocation(cloudShader.GetProgram(), "cameraPos"), 1, (float*)&tempPos);
	Vector3 tempDir = camera->GetSunDirection();
	glUniform3fv(glGetUniformLocation(cloudShader.GetProgram(), "sunDirection"), 1, (float*)&tempDir);
	//Control parameters
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "globalCoverage"), cloudModel->globalCoverage);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "globalDensity"), cloudModel->globalDensity);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "cloudScale"), cloudModel->cloudScale);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "cloudOffset"), cloudModel->cloudOffset);
	glUniform1i(glGetUniformLocation(cloudShader.GetProgram(), "sampleSteps"), cloudModel->sampleSteps);
	glUniform1i(glGetUniformLocation(cloudShader.GetProgram(), "lightSampleSteps"), cloudModel->lightSampleSteps);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "firstRayMarchingFactor"), cloudModel->firstRayMarchingFactor);
	glUniform1f(glGetUniformLocation(cloudShader.GetProgram(), "secondRayMarchingFactor"), cloudModel->secondRayMarchingFactor);
	
	cloudShader.Draw();

	glBindTextureUnit(0, 0);
	glUseProgram(0);
}

void Renderer::CreateCloudCS()
{
	if (!toolerShader.SetShader("shader/TextureOnlyVShader.glsl", "shader/TextureOnlyFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	toolerShader.SetMeshAsQuad();

	cloudModel.reset(new atmosphere::Cloud(128));

	glGenTextures(1, &cloudTex);
	glBindTexture(GL_TEXTURE_2D, cloudTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Binding Texture image uints
	glUseProgram(cloudCS.GetProgram());
	glBindImageTexture(0, renderFBO->GetColorTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	//glBindImageTexture(1, cloudModel->GetBaseShapeTex(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	//glBindImageTexture(2, cloudModel->GetDetailShapeNoiseTex(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	//glBindImageTexture(3, cloudModel->GetWeatherMapTex(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	//glBindImageTexture(4, cloudModel->GetBlueNoiseTex(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
	glBindTextureUnit(6, cloudModel->GetBaseShapeTex());
	glBindTextureUnit(7, cloudModel->GetDetailShapeNoiseTex());
	glBindTextureUnit(8, cloudModel->GetWeatherMapTex());
	glBindTextureUnit(9, cloudModel->GetBlueNoiseTex());
	glBindImageTexture(5, cloudTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	//Resolution
	Vector2 resolution(width, height);
	glUniform2fv(glGetUniformLocation(cloudCS.GetProgram(), "resolution"), 1, (float*)&resolution);
	Vector2 projFactor(1.f/projMatrix.values[0], 1.f/projMatrix.values[5]);
	glUniform2fv(glGetUniformLocation(cloudCS.GetProgram(), "projFactor"), 1, (float*)&projFactor);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "cloudLayerRadius"), cloudModel->cloudLayerRadius);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "cloudHeightAboveGround"), cloudModel->cloudHeightAboveGround);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "cloudLayerLength"), cloudModel->cloudLayerLength);

	glUseProgram(0);
}

void Renderer::RenderCloudCS()
{
	glUseProgram(cloudCS.GetProgram());
	

	cameraMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(cloudCS.GetProgram(), "viewMatrix"), 1, true, (float*)&cameraMatrix);
	Vector3 tempPos = camera->GetPosition();
	glUniform3fv(glGetUniformLocation(cloudCS.GetProgram(), "cameraPos"), 1, (float*)&tempPos);
	Vector3 tempDir = camera->GetSunDirection();
	glUniform3fv(glGetUniformLocation(cloudCS.GetProgram(), "sunDirection"), 1, (float*)&tempDir);
	//Control parameters
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "globalCoverage"), cloudModel->globalCoverage);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "globalDensity"), cloudModel->globalDensity);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "cloudScale"), cloudModel->cloudScale);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "cloudOffset"), cloudModel->cloudOffset);
	glUniform1i(glGetUniformLocation(cloudCS.GetProgram(), "sampleSteps"), cloudModel->sampleSteps);
	glUniform1i(glGetUniformLocation(cloudCS.GetProgram(), "lightSampleSteps"), cloudModel->lightSampleSteps);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "firstRayMarchingFactor"), cloudModel->firstRayMarchingFactor);
	glUniform1f(glGetUniformLocation(cloudCS.GetProgram(), "secondRayMarchingFactor"), cloudModel->secondRayMarchingFactor);

	glDispatchCompute(ceil(width / 8), ceil(height / 8), 1);

	glUseProgram(0);

	glUseProgram(toolerShader.GetProgram());
	glBindTextureUnit(0, cloudTex);
	toolerShader.Draw();
	glBindTextureUnit(0, 0);
	glUseProgram(0);
}

void Renderer::CreateAtmosphericScatteringModel()
{
	//These are all parameter needed for atmospheric scattering model.
	enum Luminance { NONE, APPROXIMATE, PRECOMPUTED };
	constexpr double kPi = 3.1415926;
	constexpr double kSunAngularRadius = 0.00935 / 2.0;
	constexpr double kSunSolidAngle = kPi * kSunAngularRadius * kSunAngularRadius;
	constexpr double kLengthUnitInMeters = 1000.0;

	bool	use_constant_solar_spectrum_ = false;
	bool	use_ozone_ = true;
	bool	use_combined_textures_ = true;
	bool	use_half_precision_ = true;
	bool	use_luminance_ = APPROXIMATE;
	bool	do_white_balance_ = false;

	constexpr int kLambdaMin = 360;
	constexpr int kLambdaMax = 830;
	constexpr double kSolarIrradiance[48] = {
	  1.11776, 1.14259, 1.01249, 1.14716, 1.72765, 1.73054, 1.6887, 1.61253,
	  1.91198, 2.03474, 2.02042, 2.02212, 1.93377, 1.95809, 1.91686, 1.8298,
	  1.8685, 1.8931, 1.85149, 1.8504, 1.8341, 1.8345, 1.8147, 1.78158, 1.7533,
	  1.6965, 1.68194, 1.64654, 1.6048, 1.52143, 1.55622, 1.5113, 1.474, 1.4482,
	  1.41018, 1.36775, 1.34188, 1.31429, 1.28303, 1.26758, 1.2367, 1.2082,
	  1.18737, 1.14683, 1.12362, 1.1058, 1.07124, 1.04992
	};

	constexpr double kOzoneCrossSection[48] = {
	  1.18e-27, 2.182e-28, 2.818e-28, 6.636e-28, 1.527e-27, 2.763e-27, 5.52e-27,
	  8.451e-27, 1.582e-26, 2.316e-26, 3.669e-26, 4.924e-26, 7.752e-26, 9.016e-26,
	  1.48e-25, 1.602e-25, 2.139e-25, 2.755e-25, 3.091e-25, 3.5e-25, 4.266e-25,
	  4.672e-25, 4.398e-25, 4.701e-25, 5.019e-25, 4.305e-25, 3.74e-25, 3.215e-25,
	  2.662e-25, 2.238e-25, 1.852e-25, 1.473e-25, 1.209e-25, 9.423e-26, 7.455e-26,
	  6.566e-26, 5.105e-26, 4.15e-26, 4.228e-26, 3.237e-26, 2.451e-26, 2.801e-26,
	  2.534e-26, 1.624e-26, 1.465e-26, 2.078e-26, 1.383e-26, 7.105e-27
	};

	constexpr double kDobsonUnit = 2.687e20;
	constexpr double kMaxOzoneNumberDensity = 300.0 * kDobsonUnit / 15000.0;
	constexpr double kConstantSolarIrradiance = 1.5;
	constexpr double kBottomRadius = 6360000.0;
	constexpr double kTopRadius = 6420000.0;
	constexpr double kRayleigh = 1.24062e-6;
	constexpr double kRayleighScaleHeight = 8000.0;
	constexpr double kMieScaleHeight = 1200.0;
	constexpr double kMieAngstromAlpha = 0.0;
	constexpr double kMieAngstromBeta = 5.328e-3;
	constexpr double kMieSingleScatteringAlbedo = 0.9;
	constexpr double kMiePhaseFunctionG = 0.8;
	constexpr double kGroundAlbedo = 0.1;
	const double max_sun_zenith_angle =
		(use_half_precision_ ? 102.0 : 120.0) / 180.0 * kPi;

	atmosphere::DensityProfileLayer
		rayleigh_layer(0.0, 1.0, -1.0 / kRayleighScaleHeight, 0.0, 0.0);
	atmosphere::DensityProfileLayer mie_layer(0.0, 1.0, -1.0 / kMieScaleHeight, 0.0, 0.0);
	std::vector<atmosphere::DensityProfileLayer> ozone_density;
	ozone_density.push_back(
		atmosphere::DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0));
	ozone_density.push_back(
		atmosphere::DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0));

	std::vector<double> wavelengths;
	std::vector<double> solar_irradiance;
	std::vector<double> rayleigh_scattering;
	std::vector<double> mie_scattering;
	std::vector<double> mie_extinction;
	std::vector<double> absorption_extinction;
	std::vector<double> ground_albedo;
	for (int l = kLambdaMin; l <= kLambdaMax; l += 10) {
		double lambda = static_cast<double>(l) * 1e-3;
		double mie =
			kMieAngstromBeta / kMieScaleHeight * pow(lambda, -kMieAngstromAlpha);
		wavelengths.push_back(l);
		if (use_constant_solar_spectrum_) {
			solar_irradiance.push_back(kConstantSolarIrradiance);
		}
		else {
			solar_irradiance.push_back(kSolarIrradiance[(l - kLambdaMin) / 10]);
		}
		rayleigh_scattering.push_back(kRayleigh * pow(lambda, -4));
		mie_scattering.push_back(mie * kMieSingleScatteringAlbedo);
		mie_extinction.push_back(mie);
		absorption_extinction.push_back(use_ozone_ ?
			kMaxOzoneNumberDensity * kOzoneCrossSection[(l - kLambdaMin) / 10] :
			0.0);
		ground_albedo.push_back(kGroundAlbedo);
	}

	atmosphereScattering.reset(new atmosphere::AtmosphereModel(wavelengths, solar_irradiance, kSunAngularRadius,
		kBottomRadius, kTopRadius, { rayleigh_layer }, rayleigh_scattering,
		{ mie_layer }, mie_scattering, mie_extinction, kMiePhaseFunctionG,
		ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
		kLengthUnitInMeters, use_luminance_ == PRECOMPUTED ? 15 : 3,
		use_combined_textures_, use_half_precision_));
	atmosphereScattering->Init();

	//Linking shader program
	atmosphereScatteringShader.SetShader("shader/AtmosphereVShader.glsl", "shader/AtmosphereFShader.glsl");
	glAttachShader(atmosphereScatteringShader.GetProgram(), atmosphereScattering->shader());
	glLinkProgram(atmosphereScatteringShader.GetProgram());
	GLint code;
	glGetProgramiv(atmosphereScatteringShader.GetProgram(), GL_LINK_STATUS, &code);

	if (!code) {
		cout << "Shader for atmospheric scattering set up failed!" << endl;
	}

	glDetachShader(atmosphereScatteringShader.GetProgram(), atmosphereScattering->shader());
	atmosphereScatteringShader.SetMeshAsQuad();

	//Set 3 constant uniforms in fragment shader
	glUseProgram(atmosphereScatteringShader.GetProgram());
	atmosphereScattering->SetProgramUniforms(atmosphereScatteringShader.GetProgram(), 0, 1, 2, 3);
	double white_point_r = 1.0;
	double white_point_g = 1.0;
	double white_point_b = 1.0;
	if (do_white_balance_) {
		atmosphere::AtmosphereModel::ConvertSpectrumToLinearSrgb(wavelengths, solar_irradiance,
			&white_point_r, &white_point_g, &white_point_b);
		double white_point = (white_point_r + white_point_g + white_point_b) / 3.0;
		white_point_r /= white_point;
		white_point_g /= white_point;
		white_point_b /= white_point;
	}
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "white_point"),
		white_point_r, white_point_g, white_point_b);
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "earth_center"),
		0.0, 0.0, -kBottomRadius / kLengthUnitInMeters );
	glUniform2f(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "sun_size"),
		tan(kSunAngularRadius),
		cos(kSunAngularRadius));

	//Don't know why it is needed, probably due to precomputation
	glViewport(0, 0, width, height);

	glUniformMatrix4fv(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "projMatrix"), 1, GL_FALSE,
		(float*)&projMatrix);

	glUseProgram(0);
}

void Renderer::RenderAtmosphericScatteringModel()
{
	//Redundant parameter data similar to the CreateAtmosphericScatteringModel()
	enum Luminance {NONE, APPROXIMATE, PRECOMPUTED};
	bool	use_luminance_ = NONE;
	float	exposure_ = 10.0;

	Vector3 sunDir = camera->GetSunDirection();
	Vector3 cameraPos = camera->GetPosition();

	glUseProgram(atmosphereScatteringShader.GetProgram());
	//For some reasons, coordinate system in Bruneton's model is in order z,x,y
	//whereas OpenGL has x,y,z order.
	//PS: This adjustment also applies to the vertex shader when generating view ray.
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "camera"),
		cameraPos.z, cameraPos.x, cameraPos.y);
	glUniform1f(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "exposure"),
		use_luminance_ != NONE ? exposure_ * 1e-5 : exposure_);
	cameraMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "viewMatrix"),
		1, true, (float*)&cameraMatrix);
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetProgram(), "sun_direction"),
		sunDir.z, sunDir.x, sunDir.y);

	atmosphereScatteringShader.Draw();
	glUseProgram(0);
}

void Renderer::CreateDepthOfField()
{
	if (!DOFShader.SetShader("shader/EmptrVShader.glsl", "shader/DepthOfFieldFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	DOFShader.SetMeshAsQuad();

	glGenTextures(1, &outputTex);
	glBindTexture(GL_TEXTURE_2D, outputTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, height, width);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Renderer::RenderDepthOfField()
{
	SummedAreaTable(postProcessingFBO->GetColorTexture());

	glUseProgram(DOFShader.GetProgram());

	glBindTextureUnit(1, postProcessingFBO->GetColorTexture());
	glBindTextureUnit(2, postProcessingFBO->GetDepthTexture());
	glUniform1f(glGetUniformLocation(DOFShader.GetProgram(), "focalDistance"), focalDistance);
	glUniform1f(glGetUniformLocation(DOFShader.GetProgram(), "focalDepth"), focalDepth);

	DOFShader.Draw();
	glUseProgram(0);
}

void Renderer::SummedAreaTable(GLuint texture)
{
	glUseProgram(SATComputeShader.GetProgram());

	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(width, 1, 1);

	glBindImageTexture(0, outputTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(height, 1, 1);

	glUseProgram(0);
}

void Renderer::SetTransformUBO()
{
	//Buffer creation
	glCreateBuffers(1, &transformUBO);
	//coloum or row?
	glNamedBufferStorage(transformUBO, sizeof(Matrix4), (const void*)&projMatrix, GL_DYNAMIC_STORAGE_BIT);
	//Bind buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformUBO);
}

//This snippet of code is used to show the order of pixel rendering on local hardware.
//From: https://www.geeks3d.com/20120309/opengl-4-2-atomic-counter-demo-rendering-order-of-fragments/#overview
void Renderer::CreateAtomicBuffer()
{

	glGenBuffers(1, &atomicBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer);
	ResetAtomicBuffer();
}

//Reset buffer by using Map, glBufferSubData() and glClearBufferSubData are also available
void Renderer::ResetAtomicBuffer()
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer);
	GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
		0, sizeof(GLuint),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	ptr[0] = 0; //why 2?What are stored in the first two indices?
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void Renderer::CreateShadowMap()
{
	if (!shadowMappingShader.SetShader("shader/ShadowMapVShader.glsl", "shader/ShadowMapFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	shadowFBO.reset(new FrameBuffer(SHADOWWIDTH, SHADOWHEIGHT, FBOCreationType::DEPTH));
}

void Renderer::RenderShadowMap()
{
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, SHADOWWIDTH, SHADOWHEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	
	lightMatrix = Matrix4::BuildViewMatrix(light1->GetPosition(), Vector3(0, 0, 0));
	lightMatrix = projMatrix * lightMatrix * modelMatrix;

	glUseProgram(shadowMappingShader.GetProgram());
	glUniformMatrix4fv(glGetUniformLocation(shadowMappingShader.GetProgram(), "lightMatrix"), 1, GL_FALSE, (float*)&lightMatrix);

	object->Draw();

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST);
}

void Renderer::CreateTrajectory()
{
	trajectory = new GameObject();

	if (!trajectory->SetShader("shader/TriangleVShader.glsl", "shader/TriangleFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}

	trajectory->SetMesh(new Trajectory());
}


void Renderer::ImGUIInit(Window& parent)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(parent.GetHandle());
	ImGui_ImplOpenGL3_Init();
}

void Renderer::RenderText()
{
	textRenderer.RenderText("FPS: " + to_string(fps), 10.0f, height - 25.f);
	string CameraPositionText = "Camera Position: " +
		to_string(camera->GetPosition().x) + ", " +
		to_string(camera->GetPosition().y) + ", " +
		to_string(camera->GetPosition().z);
	textRenderer.RenderText(CameraPositionText, 10.0f, height - 45.f);
	textRenderer.RenderText("Camera Pitch: " + to_string(camera->GetPitch()), 10.0f, height - 65.f);
	textRenderer.RenderText("Camera Yaw: " + to_string(camera->GetYaw()), 10.0f, height - 85.f);
	textRenderer.RenderText("Sun Zenith : " + to_string(camera->GetSunZenithDegree()), 10.0f, height - 105.f);
	textRenderer.RenderText("Sun Azimuth: " + to_string(camera->GetSunAzimuthDegree()), 10.0f, height - 125.f);
}

void Renderer::RenderImGUI()
{	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//This demo showcases most of the features of Dear ImGUI
	//ImGui::ShowDemoWindow((bool*)true);

#ifdef RENDER_CLOUD
	ImGui::SliderFloat("Cloud global coverage", &cloudModel->globalCoverage, 0.0f, 1.0f);
	ImGui::SliderFloat("Cloud global density", &cloudModel->globalDensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Cloud scale", &cloudModel->cloudScale, 0.1f, 10.f);
	ImGui::SliderFloat("Cloud offset", &cloudModel->cloudOffset, 0.f, 5.f);
	ImGui::SliderFloat("First raymarching factor", &cloudModel->firstRayMarchingFactor, 0.01f, 100.f);
	ImGui::SliderFloat("Second raymarching factor", &cloudModel->secondRayMarchingFactor, 0.01f, 100.f);

	ImGui::SliderInt("Sample steps", &cloudModel->sampleSteps, 1, 128);
	ImGui::SliderInt("Light sample steps", &cloudModel->lightSampleSteps, 1, 10);
#endif

#ifdef DEPTH_OF_FIELD
	ImGui::SliderFloat("Focal Distance", &focalDistance, 0.f, 500.f);
	ImGui::SliderFloat("Focal Depth", &focalDepth, 0.f, 500.f);
#endif

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::UpdateControl(float msec)
{
	if (Window::GetKeyboard()->KeyHeld(KEYBOARD_CONTROL)) {
		Vector3 temp = light1->GetPosition();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			temp.z += msec;
			light1->SetPosition(temp);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			temp.x -= msec;
			light1->SetPosition(temp);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			temp.z -= msec;
			light1->SetPosition(temp);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			temp.x += msec;
			light1->SetPosition(temp);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			temp.y += msec;
			light1->SetPosition(temp);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			temp.y -= msec;
			light1->SetPosition(temp);
		}
	}
}

void Renderer::ComputeShaderPlayground()
{
	//ValueNoise2D();
	//PerlinNoise();
	PerlinNoiseBuffer();
}

void Renderer::ScreenShot(std::string filename)
{
	int row_size = ((width * 3 + 3) & ~3);
	int data_size = row_size * height;
	unsigned char* data = new unsigned char[data_size];
	glReadPixels(0,0,width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(("../demo/" + filename + ".jpg").c_str(), width, height, 3, data, 100);
	delete[] data;
}