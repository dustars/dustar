#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout (binding = 0, rgba32f) readonly uniform image2D renderFBO;
//layout (binding = 1, rgba32f) readonly uniform image3D cloudBaseTexture;
//layout (binding = 2, rgba32f) readonly uniform image3D cloudDetailTexture;
//ayout (binding = 3, rgba8) readonly uniform image2D weatherMap;
//layout (binding = 4, r32f) readonly uniform image2D blueNoiseTexture;
//Result Image
layout (binding = 5, rgba32f) writeonly uniform image2D cloudTex;
layout(binding = 6) uniform sampler3D cloudBaseTexture;
layout(binding = 7) uniform sampler3D cloudDetailTexture;
layout(binding = 8) uniform sampler2D weatherMap;
layout(binding = 9) uniform sampler2D blueNoiseTexture;

//View-related parameters
uniform vec3 cameraPos;
uniform mat4 viewMatrix;
uniform vec2 projFactor;
//Possible image resolution needed
uniform vec2 resolution;
//Cloud-related parameters
uniform float globalCoverage;
uniform float globalDensity;
uniform float cloudScale;
uniform float cloudOffset;
uniform float cloudLayerRadius;
uniform float cloudHeightAboveGround;
uniform float cloudLayerLength;

uniform int sampleSteps;
uniform int lightSampleSteps;

uniform vec3 sunDirection;
uniform float firstRayMarchingFactor = 1.f;
uniform float secondRayMarchingFactor = 1.f;

//Light parameters
vec3 lightRay = normalize(vec3(10.f, 10.f, 10.f));
vec3 lightColor = vec3(1.f, 1.0f, 1.0f);
float eccentricityG = 0.2f;

//Inner and outer spheres
vec4 cloudInnerSphere = vec4(0, cloudHeightAboveGround - cloudLayerRadius, 0, cloudLayerRadius);
vec4 cloudOuterSphere = vec4(cloudInnerSphere.xyz, cloudLayerRadius + cloudLayerLength);

//Original: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool RaySphereIntersectionTest(vec3 rayDir, out vec3 intersecPoint, out float intervalDist) {

	vec3 L = cameraPos - cloudInnerSphere.xyz;
	//Make sure the camera is inside the sphere.
	if (length(L) > cloudInnerSphere.w) return false;
	float projLength = dot(L, rayDir);
	float cToRay2 = dot(L, L) - projLength * projLength;
	//if the ray doesn't hit the sphere
	//if (cToRay2 > (cloudInnerSphere.w * cloudInnerSphere.w)) return false;

	//Inner sphere intersection point
	float tempInner = sqrt(cloudInnerSphere.w * cloudInnerSphere.w - cToRay2);
	intersecPoint = cameraPos + (tempInner - projLength) * rayDir;

	//When intersection point above 0, it's visible, otherwise not.
	if (intersecPoint.y < 0) return false;

	//Calculate the length from camera to the outer sphere intersection point
	float tempOuter = sqrt(cloudOuterSphere.w * cloudOuterSphere.w - cToRay2);
	//distance between two intersection points
	intervalDist = tempOuter - tempInner;

	return true;
}

//Remapping function
float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

//Shape-altering height function.
//It makes bottom thick, and makes top thin.
float ShapeHeightFunc(float ph, float b) {
	float SRbottom = clamp(map(ph, 0.f, 0.07f, 0.f, 1.f), 0.f, 1.f);
	float SRtop = clamp(map(ph, b * 0.4f, b, 1.f, 0.f), 0.f, 1.f);
	return SRbottom * SRtop;
}

//Density-altering height function.
//It makes bottom less tranparent than the top.
float DensityHeightFunc(float ph, float a) {
	float DRbottom = ph * clamp(map(ph, 0.f, 0.15f, 0.f, 1.f), 0.f, 1.f);
	float DRtop = clamp(map(ph, 0.9f, 1.f, 1.f, 0.f), 0.f, 1.f);
	return globalDensity * DRbottom * DRtop * a * 2;
}

//Sample the FBM detail noise.
//The maximum influence it has is limited to 0.35, with a natural
//number to apply the effect of globalCoverage as well.
//The interpolation at return is for a more "rounded" shape.
float SampleDetailNoise(vec3 uvw, float pHeight) {
	vec3 rawDetailNoise = texture(cloudDetailTexture, uvw).rgb;
	float fbmDetail = rawDetailNoise.r * 0.625f + rawDetailNoise.g * 0.25f + rawDetailNoise.b * 0.125f;

	return 0.35f * exp(-globalCoverage * 0.75f) *
		mix(fbmDetail, 1.f - fbmDetail, clamp(pHeight * 5, 0.f, 1.f));
}

//Sample the base shape of the cloud
float SampleDensity(vec3 pos, float pHeight, bool sampleDetail) {
	vec3 uvw = pos * 0.0001 * cloudScale + 0.5 + cloudOffset * 0.1;

	//Sample the raw data from noise texture.
	vec4 rawData = texture(cloudBaseTexture, uvw);
	float fbmWorley = rawData.g * 0.625f + rawData.b * 0.25f + rawData.a * 0.125f;
	float baseNoise = map(rawData.r, -(1.f - fbmWorley), 1.f, 0.f, 1.f);

	if (sampleDetail == true) {
		//Currently no alpha channel for the weather map
		//Hard-coded wmAlpha = 1 to represent no density reduction.
		vec3 wm = texture(weatherMap, uvw.xz).xyz;
		float wmAlpha = 1.f;

		//float pHeight = GetHeightPercentage(pos);
		//Shape-altering height function
		float heightModified = baseNoise;// *ShapeHeightFunc(pHeight, wm.b);

		//Use Weather map to control the coverage of cloud
		float weatherMapFactor = globalCoverage * max(wm.r, clamp(globalCoverage - 0.5f, 0.f, 1.f) * wm.g * 2.f);

		float wmModified = clamp(map(heightModified, 1.f - weatherMapFactor, 1.f, 0.f, 1.f), 0.f, 1.f);

		wmModified = clamp(map(wmModified, SampleDetailNoise(uvw, pHeight), 1.f, 0.f, 1.f), 0.f, 1.f);
		//Density-altering height function at last
		return wmModified;// *DensityHeightFunc(pHeight, wmAlpha);
	}

	return baseNoise;
}

float HenyeyGreenstein(float cosAngle, float g)
{
	float g2 = g * g;
	return ((1.0 - g2) / pow((1.0 + g2 - 2.0 * g * cosAngle), 1.5)) / 4.0 * 3.1415;
}

float lightCalculation(float density, vec3 rayDir, float blueNoise)
{
	float cosAngle = clamp(dot(sunDirection, rayDir), 0.f, 1.f);
	float prim = exp(-6 * density);
	float scnd = exp(-6 * 0.2) * 0.7;
	float checkval = map(cosAngle, 0.0, 1.0, scnd, scnd * 0.5);
	float atten = max(checkval, prim) + blueNoise * 0.0003f;
	float outScattering = 1.f - exp(-density * 20.f);
	return 2 * atten * outScattering * HenyeyGreenstein(cosAngle, eccentricityG);

}

float lightIntensity() {

	return 1.f;
}

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);//Current pixel
	//vec2 uv = (p - .5 * resolution.xy) / resolution.y;
	vec2 uv = vec2(p) / resolution - 0.5f;
	uv *= projFactor;
	vec3 rayDir = mat3(viewMatrix) * normalize(vec3(uv.x, -uv.y, -0.5)); //why 0.5????

	vec3 color = imageLoad(renderFBO, ivec2(p.x, resolution.y - p.y)).xyz;

	vec3 pos;								//Intersection point
	float intervalDist;						//Length to march
	
	if (RaySphereIntersectionTest(rayDir, pos, intervalDist)) {
		float density = 0.f;
		float radiance = 0.f;
		float T = 1.f; //transmittance

		//Cause of color banding artifact
		float stepLength = cloudLayerLength / sampleSteps;
		vec3 stepDir = stepLength * rayDir;

		//For optimizations
		float densityTest = 0.f;
		int zeroDensityCount = 0;
		float blueNoise = texture(blueNoiseTexture, pos.xz / 10.f).x;
		pos += (blueNoise - 0.5) * 2 * stepDir; //Introduce randomness

		int totalSteps = int(intervalDist / stepLength);
		for (int i = 0; i < totalSteps; i++) { //Primary raymarching

			float pHeight = float(i) / totalSteps;
			//float pHeight = pos.y / cloudHeightAboveGround;

			if (densityTest > 0.f) { //Full sampling, otherwise cheap sampling
				float sampleDensity = SampleDensity(pos, pHeight, true);
				if (sampleDensity == 0) {
					zeroDensityCount++;
				}

				if (zeroDensityCount != 6) { //early exit check
					if (sampleDensity != 0) {

						density += sampleDensity;
						T *= exp(-density * stepLength * firstRayMarchingFactor * 0.001);

						float lightRayDensity = 0.f;
						float Ts = 1.f; //transmittance for sample point and sun

						vec3 lightSamplePos = pos;
						float sunStepLength = 0.5f * cloudLayerLength / lightSampleSteps;
						vec3 sunStepSize = sunStepLength * sunDirection;

						for (int j = 0; j < lightSampleSteps; j++) { //Secondary raymarching
							lightRayDensity += SampleDensity(lightSamplePos, pHeight, true);
							Ts *= exp(-lightRayDensity * sunStepLength * secondRayMarchingFactor * 0.01);
							lightSamplePos += sunStepSize;
						}
						float sunRadiance = 10; //or 10?
						float inScatteringRadiance = Ts * HenyeyGreenstein(clamp(dot(sunDirection, rayDir), 0.f, 1.f), eccentricityG) * sunRadiance;
						//float inScatteringRadiance = Ts * lightCalculation(lightRayDensity, sunDirection, blueNoise) * sunRadiance;

						radiance += T * 0.05 * inScatteringRadiance * stepLength;

						//radiance += density * lightCalculation(densityAloneLightRay, sunDirection, blueNoise) * T;

						if (T < 0.001f) break; //Early exit if converging.
					}
					pos += stepDir;
				}
				else {
					densityTest = 0.f;
					zeroDensityCount = 0;
				}
			}
			else {
				densityTest = SampleDensity(pos, pHeight, false);
				if (densityTest == 0) pos += stepDir;
				else pos -= stepDir;
			}
		} //end of ray marching
	
		vec3 cloudColor = pow(vec3(1.f) - exp(-radiance), vec3(1.0/2.2));
		color = mix(cloudColor, color, T);
	}

	imageStore(cloudTex, p, vec4(color, 1.f));
}