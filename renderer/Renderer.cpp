#include "Renderer.h"

extern inline void SaveAsPicture(const char* filename, std::size_t width, std::size_t height, std::size_t comp, const void* data);

Renderer::Renderer(Window& parent)
	: RenderBase(parent),
	renderFBO(new GeneralFrameBuffer(width, height)),
	textRenderer(TextRenderer(width, height))
{
#ifdef TESTING
	camera = new Camera(0.f, 180.f, Vector3(0.f, 0.f, -10.f));

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

#ifdef TESTING_OBJECT
	object = new RenderObject();
	if (!object->SetShader("shader/TextureOnlyVShader.glsl", "shader/TextureOnlyFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	if (!object->GetTexture()->SetTexture("../assets/Textures/container.jpg")) {
		cout << "Texture set up failed!" << endl;
	}
	object->GetMesh()->CreatePlane();
	//noiseGeneration();
	//object->GetTexture()->SetTexture(noiseTex);
#endif // TESTING_OBJECT

	modelMatrix = modelMatrix * Matrix4::Scale(Vector3(10.f, 10.f, 10.f));
	pointLight1 = new PointLight(Vector4(0.f, 0.f, 500.f, 1.f), Vector4(0.9f, 0.8f, 0.4f, 1.f));

#ifdef ATMOSPHERE
	CreateAtmosphericScatteringModel();
#else
	CreateSkybox();
#endif // ATMOSPHERE

#ifdef RENDER_CLOUD
	CreateCloud();
#endif // RENDER_CLOUD

	init = true;
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
	object->SetMesh(new HeightMap(5, 2, 0.707, MAPWIDTH, MAPLENGTH));

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
#endif // TESTING
}

Renderer::~Renderer() {
	if (camera)						delete camera;
	if (object)						delete object;
	if (trajectory)					delete trajectory;
	if (skybox)						delete skybox;
	if (pointLight1)				delete pointLight1;
	if (particleMaster)				delete particleMaster;
}

void Renderer::Update(float dt) {

	// Todo: Put into the 60 fps loop
	camera->UpdateCamera(dt);

	// Temporary method to limit frame rate at 60FPS
	oneFramePerMilliSecond += dt;
	if (oneFramePerMilliSecond > renderFrames) {
		////////////////////////
		//  Locations Update  //
		////////////////////////
		if (particleMaster) {
			particleMaster->Update(oneFramePerMilliSecond);
		}

		if (trajectory) {
			trajectory->GetMesh()->Update(oneFramePerMilliSecond);
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
		oneFramePerSecond += oneFramePerMilliSecond;
		frameCount++;
		oneFramePerMilliSecond = 0;
		if (oneFramePerSecond > 1000.f) {
			oneFramePerSecond = 0;
			fps = frameCount;
			frameCount = 0;
		}
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

void Renderer::TestRendering()
{
#ifdef RENDER_CLOUD
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO->GetFrameBuffer());
#endif
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

#ifdef ATMOSPHERE
	if (atmosphereScattering.get()) RenderAtmosphericScatteringModel();
#else
	if (skybox) renderSkyBox();
#endif // ATMOSPHERE

	if (object) {
		glEnable(GL_DEPTH_TEST);
		renderObject();
		glDisable(GL_DEPTH_TEST);
	}

#ifdef RENDER_CLOUD
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderCloud();
#endif

#ifdef OFFLINE
	ScreenShot("Offline_Rendering");
#else
	if (isRenderingText) RenderText();
	::SwapBuffers(deviceContext);
#endif
}


void Renderer::renderObject()
{
	glUseProgram(object->GetShader()->GetProgram());
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
}

void Renderer::CreateCloud()
{
	if (!cloudShader.SetShader("shader/CloudVShader.glsl", "shader/CloudFShader.glsl")) {
		cout << "Shader set up failed!" << endl;
	}
	cloudShader.GetMesh()->CreatePlane(); //ray marching plane

	cloudModel.reset(new atmosphere::Cloud(128, true));

	//glUseProgram(cloudShader.GetShader()->GetProgram());
	//glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "ProjMatrix"), 1, GL_FALSE, (float*)&projMatrix);
	//glUseProgram(0);
}

void Renderer::RenderCloud()
{
	glUseProgram(cloudShader.GetShader()->GetProgram());

	glBindTextureUnit(0, renderFBO->GetColorTexture());
	glBindTextureUnit(1, renderFBO->GetDepthTexture());
	glBindTextureUnit(2, cloudModel->GetBaseShapeTex());
	glBindTextureUnit(3, cloudModel->GetDetailShapeNoiseTex());

	//glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "ModelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "ViewMatrix"), 1, GL_FALSE, (float*)&viewMatrix);

	glUniform2fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "resolution"), 1, (float*)&Vector2(width, height));
	glUniform3fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());;
	Matrix4 rotationMatrix = Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(camera->GetPitch(), Vector3(1, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(cloudShader.GetShader()->GetProgram(), "rotationMatrix"), 1, GL_FALSE, (float*)&rotationMatrix);

	cloudShader.Draw();
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
	bool	use_luminance_ = NONE;
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

	atmosphereScattering.reset(new atmosphere::Model(wavelengths, solar_irradiance, kSunAngularRadius,
		kBottomRadius, kTopRadius, { rayleigh_layer }, rayleigh_scattering,
		{ mie_layer }, mie_scattering, mie_extinction, kMiePhaseFunctionG,
		ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
		kLengthUnitInMeters, use_luminance_ == PRECOMPUTED ? 15 : 3,
		use_combined_textures_, use_half_precision_));
	atmosphereScattering->Init();

	//Linking shader program
	atmosphereScatteringShader.SetShader("shader/AtmosphereVShader.glsl", "shader/AtmosphereFShader.glsl");
	glAttachShader(atmosphereScatteringShader.GetShader()->GetProgram(), atmosphereScattering->shader());
	glLinkProgram(atmosphereScatteringShader.GetShader()->GetProgram());
	GLint code;
	glGetProgramiv(atmosphereScatteringShader.GetShader()->GetProgram(), GL_LINK_STATUS, &code);

	if (!code) {
		cout << "Shader for atmospheric scattering set up failed!" << endl;
	}

	glDetachShader(atmosphereScatteringShader.GetShader()->GetProgram(), atmosphereScattering->shader());
	atmosphereScatteringShader.GetMesh()->CreatePlane();

	//Set 3 constant uniforms in fragment shader
	glUseProgram(atmosphereScatteringShader.GetShader()->GetProgram());
	atmosphereScattering->SetProgramUniforms(atmosphereScatteringShader.GetShader()->GetProgram(), 0, 1, 2, 3);
	double white_point_r = 1.0;
	double white_point_g = 1.0;
	double white_point_b = 1.0;
	if (do_white_balance_) {
		atmosphere::Model::ConvertSpectrumToLinearSrgb(wavelengths, solar_irradiance,
			&white_point_r, &white_point_g, &white_point_b);
		double white_point = (white_point_r + white_point_g + white_point_b) / 3.0;
		white_point_r /= white_point;
		white_point_g /= white_point;
		white_point_b /= white_point;
	}
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "white_point"),
		white_point_r, white_point_g, white_point_b);
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "earth_center"),
		0.0, 0.0, -kBottomRadius / kLengthUnitInMeters);
	glUniform2f(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "sun_size"),
		tan(kSunAngularRadius),
		cos(kSunAngularRadius));

	//Don't know why it is needed, probably due to precomputation
	glViewport(0, 0, width, height);

	const float kFovY = 45.0 / 180.0 * kPi;
	const float kTanFovY = tan(kFovY / 2.0);
	float aspect_ratio = static_cast<float>(width) / height;

	// Transform matrix from clip space to camera space (i.e. the inverse of a
	// GL_PROJECTION matrix).
	float view_from_clip[16] = {
	  kTanFovY * aspect_ratio, 0.0, 0.0, 0.0,
	  0.0, kTanFovY, 0.0, 0.0,
	  0.0, 0.0, 0.0, -1.0,
	  0.0, 0.0, 1.0, 1.0
	};
	glUniformMatrix4fv(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "projMatrix"), 1, true,
		view_from_clip);

	//glUniformMatrix4fv(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "projMatrix"), 1, GL_FALSE,
	//	(float*)&projMatrix);

	glUseProgram(0);
}

void Renderer::RenderAtmosphericScatteringModel()
{
	//Redundant parameter data similar to the CreateAtmosphericScatteringModel()
	enum Luminance {NONE, APPROXIMATE, PRECOMPUTED};
	bool	use_luminance_ = NONE;
	float	view_zenith_angle_radians_ = DegToRad(camera->GetPitch() + 90.f);	//1.47
	float	view_azimuth_angle_radians_ = DegToRad(camera->GetYaw());			//-0.1
	float	sun_zenith_angle_radians_ = 1.3;
	float	sun_azimuth_angle_radians_ = 2.9;
	float	exposure_ = 10.0;
	
	float cos_z = cos(view_zenith_angle_radians_);
	float sin_z = sin(view_zenith_angle_radians_);
	float cos_a = cos(view_azimuth_angle_radians_);
	float sin_a = sin(view_azimuth_angle_radians_);
	float ux[3] = { -sin_a, cos_a, 0.0 };
	float uy[3] = { -cos_z * cos_a, -cos_z * sin_a, sin_z };
	float uz[3] = { sin_z * cos_a, sin_z * sin_a, cos_z };

	// Transform matrix from camera frame to world space (i.e. the inverse of a
	// GL_MODELVIEW matrix).
	float model_from_view[16] = {
	  ux[0], uy[0], uz[0], uz[0],
	  ux[1], uy[1], uz[1], uz[1],
	  ux[2], uy[2], uz[2], uz[2],
	  0.0, 0.0, 0.0, 1.0
	};

	glUseProgram(atmosphereScatteringShader.GetShader()->GetProgram());
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "camera"),
		camera->GetPosition().x,
		camera->GetPosition().z,
		camera->GetPosition().y);
	glUniform1f(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "exposure"),
		use_luminance_ != NONE ? exposure_ * 1e-5 : exposure_);
	glUniformMatrix4fv(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "model_from_view"),
		1, true, model_from_view);
	glUniform3f(glGetUniformLocation(atmosphereScatteringShader.GetShader()->GetProgram(), "sun_direction"),
		cos(sun_azimuth_angle_radians_) * sin(sun_zenith_angle_radians_),
		sin(sun_azimuth_angle_radians_) * sin(sun_zenith_angle_radians_),
		cos(sun_zenith_angle_radians_));

	atmosphereScatteringShader.Draw();
	glUseProgram(0);
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

void Renderer::noiseGeneration()
{
	glGenTextures(1, &noiseTex);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::size_t res = 512;
	PerlinNoise perlin;
	WorleyNoise worley(res, 8);

	unsigned char* data = new unsigned char[res * res];


	for (std::size_t y = 0; y < res; y++) {
		for (std::size_t x = 0; x < res; x++) {
			float worleyNoise = worley.FBMNoise(x, y, 0, 3, 2.f, 0.707f);
			float perlinNoise = perlin.FBMPerlin( static_cast<double>(x) / res * 3.0,
				static_cast<double>(y) / res * 3.0, 0, 7, 2, 0.707);

			perlinNoise = Remap(perlinNoise, 0, 1.2, 0, 1);
			float temp = Clamp(Remap(perlinNoise, worleyNoise, 1.f, 0.f, 1.f), 0.f, 1.f);

			data[y * res + x] = static_cast<unsigned char>(temp * 255);
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, res, res, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<void*>(data));
	SaveAsPicture("../demo/PerlinWorleyNoise.jpg", res, res, 1, static_cast<void*>(data));

	delete[] data;
}

void Renderer::ScreenShot(std::string filename)
{
	int row_size = ((width * 3 + 3) & ~3);
	int data_size = row_size * height;
	unsigned char* data = new unsigned char[data_size];
	glReadPixels(0,0,width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	SaveAsPicture(("../demo/" + filename + ".jpg").c_str(), width, height, 3, data);
	delete[] data;
}