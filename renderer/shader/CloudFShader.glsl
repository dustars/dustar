/*
	Description:
	The Cloud fragment shader.
	Rendering steps:
	1. Get view rays
	2. Do the sphere intersection test.
	3. At the intersection point, start the ray marching.

	Created:
	2/12/2020
	Last Updated:
	7/28/2020
*/


#version 450 core

#define MAX_STEPS 50
#define MAX_DIST 20000.
#define SURF_EPSILON .01

#define NUM_SPHERES 4

//Rasterization rendering FBO
layout(binding = 0) uniform sampler2D renderFBO;
layout(binding = 1) uniform sampler2D renderDepth;

//Cloud textures
layout(binding = 2) uniform sampler3D cloudBaseTexture;
layout(binding = 3) uniform sampler3D cloudDetailTexture;
layout(binding = 4) uniform sampler2D weatherMap;

//View-related parameters
uniform vec3 cameraPos;
uniform vec2 resolution;
uniform mat4 rotationMatrix;

float imageAspectRatio = resolution.x / resolution.y;
float fov = tan(45.f / 2 * 3.1415925 / 180);

//Cloud-related parameters
uniform float globalCoverage;
uniform float globalDensity;
//x for max, y for min
uniform vec2 cloudMaxMinHeight;

int baseSteps = 20;

vec3 lightRay = vec3(2.f, 2.f, -1.f);
float eccentricityG = 0.2f;
//Cloud layer is 2km above the origin, and the Vanishing point is 10km,
//which gives a sphere located -23880m below the origin with a 25880 radius.
vec4 cloudInnerSphere = vec4(0, -5000, 0, 6500);
vec4 cloudOuterSphere = vec4(0, -5000, 0, 6500 + (cloudMaxMinHeight.x - cloudMaxMinHeight.y));


in VS{
	vec2 texCoord;
	//vec3 viewRay;
}IN;

out vec4 FragColor;

float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

//calculate ph, the height percentage of where the sampled position is in the cloud layer.
float GetCloudHeightPercentage(float height) {
	return clamp( (height - cloudMaxMinHeight.y)/(cloudMaxMinHeight.x - cloudMaxMinHeight.y) , 0.f, 1.f);
}

//Human-defined coverage of the cloud.
//r for cumulus, g for stratus.
float WeatherMapCoverage(float r, float g) {
	return max(r ,clamp(globalCoverage - 0.5f ,0.f, 1.f) * g * 2 );
}

//Shape-altering height function, determine the height range of where cloud will appear.
//SRb for bottom probablity, SRt for top probablity.
//b is the weather map control factor in blue channel.
float ShapeHeightFunc(float ph, float b) {
	float SRbottom = clamp(map(ph, 0.f, 0.07f, 0.f, 1.f), 0.f, 1.f);
	//The original paper has added 0.12 to b, why?
	b += 0.12;
	float SRtop = clamp(map(ph, b * 0.2, b, 1.f, 0.f), 0.f, 1.f);
	return SRbottom * SRtop;
}

//Density-altering height function, determine the density separately for bottom and top
//layer of the cloud, fluffy for former and dense for latter.
//a is the weather map control factor in alpha channel.
float DensityHeightFunc(float ph, float a) {
	float DRbottom = ph * clamp(map(ph, 0.f, 0.15f, 0.f, 1.f), 0.f, 1.f);
	float DRtop = clamp(map(ph, 0.9f, 1.f, 1.f, 0.f), 0.f, 1.f);
	return globalDensity * DRbottom * DRtop * a * 2;
}

//Sample the base shape of the cloud
float SampleBaseShape(vec3 uvw, float ph) {
	vec4 rawData = texture(cloudBaseTexture, uvw);
	float fbmWorley = rawData.g * 0.625f + rawData.b * 0.25f + rawData.a * 0.125f;
	float SNsample = map(rawData.r, fbmWorley - 1.f, 1.f, 0.f, 1.f);

	//I don't have an alpha channel, so...
	vec3 wm = texture(weatherMap, uvw.xy).xyz;
	float wmAlpha = 0.6f;
	
	//Shape-altering height function
	float heightModified = SNsample * ShapeHeightFunc(ph, wm.b);

	//Use Weather map to control the coverage of cloud
	//0.5f is where weather map should be.
	float weatherMapFactor = 1 - globalCoverage * 0.5f;
	float wmModified = clamp(map(heightModified, weatherMapFactor, 1.f, 0.f, 1.f), 0.f, 1.f);

	//Lastly control its density and return
	return wmModified * DensityHeightFunc(uvw.z, wma);
}

float SampleDensity(vec3 pos)
{
	//Not a good way to calculate uvw coordinate but I'll take it...
	vec3 uvw = pos / cloudOuterSphere.w + 0.5;
	float ph = GetCloudHeightPercentage(length(pos));
	return SampleBaseShape(uvw, ph);
	//Detail

	//Test
	//return max(0.f, texture(cloudBaseTexture, uvw).x - 0.5f);
}

float HenyeyGreenstein(vec3 inLightVector, vec3 inViewVector, float inG)
{
	float cosAngle = dot(normalize(inLightVector), normalize(inViewVector));
	return ((1.0 - inG * inG) / pow((1.0 + inG * inG - 2.0 * inG * cosAngle), 3.0 / 2.0)) / 4.0 * 3.1415;
}

float lightCalculation(float density, vec3 rayDir)
{
	float beersLaw = exp(-density);
	float outScattering = 1.f - exp(-density);
	float lightEnergy = 2 * beersLaw * outScattering * HenyeyGreenstein(lightRay, rayDir, eccentricityG);
	return lightEnergy;
}

//vec3 NormalCalculation(vec3 pos)
//{
//	float d = GetSDFSphere(pos);
//	vec2 e = vec2(.01, 0);
//
//	vec3 n = d - vec3(
//		GetSDFSphere(pos - e.xyy),
//		GetSDFSphere(pos - e.yxy),
//		GetSDFSphere(pos - e.yyx));
//
//	return normalize(n);
//}
//
//float LightCalculation(vec3 pos)
//{
//	//hard-coded light
//	vec3 lightPos = vec3(0, 10, 0);
//	vec3 lDir = normalize(lightPos - pos);
//	vec3 normal = NormalCalculation(pos);
//
//	float cos = clamp(dot(lDir, normal), 0., 1.);
//	//float shadowDis = RayMarch(pos+ normal *SURF_EPSILON*2., lDir);
//	//if (shadowDis < length(lightPos - pos)) cos *= .1;
//	return cos;
//}

void main(void)
{
	vec3 color = texture(renderFBO, IN.texCoord).xyz;
	float depth = texture(renderDepth, IN.texCoord).x;

	//Inverse transformation on non-linear depth value to get linear z-value
	depth = depth * 2.f - 1.f;
	depth = 40000.f / (20001.f - depth * 19999.f); //harded-code near and far plane, see https://learnopengl.com/Advanced-OpenGL/Depth-testing

	//1. Calculate view ray
	vec2 uv;
	uv.x = (2 * ((gl_FragCoord.x + 0.5) / resolution.x) - 1) * fov * imageAspectRatio;
	uv.y = (2 * ((gl_FragCoord.y + 0.5) / resolution.y) - 1) * fov;
	vec3 rayPos = cameraPos;
	vec3 rayDir = normalize(( ( rotationMatrix * vec4(normalize(vec3(uv, -1)), 1)) ).xyz);

	//2. Ray tracing to determine the inner and outer sphere
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	//Two solutions are: x0 = q / a, x1 = c / q;
	vec3 L = cameraPos - cloudInnerSphere.xyz;
	float a = dot(rayDir, rayDir);
	float b = 2 * dot(L, rayDir);
	float c = dot(L, L) - cloudInnerSphere.w * cloudInnerSphere.w;
	float discr = b * b - 4 * a * c;
	float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
	vec3 innerIntersecPoint = rayPos + (c / q) * rayDir;

	L = cameraPos - cloudOuterSphere.xyz;
	b = 2 * dot(L, rayDir);
	c = dot(L, L) - cloudOuterSphere.w * cloudOuterSphere.w;
	discr = b * b - 4 * a * c;
	q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
	vec3 outerIntersecPoint = rayPos + (c / q) * rayDir;
	
	float rayDist = length(innerIntersecPoint);
	float layerDist = length(outerIntersecPoint - innerIntersecPoint);

	//3. Ray marching for the cloud interval
	//Do a little test to cut out rays that cannot be seen.
	if (rayDist < depth && c/q > 0 && innerIntersecPoint.y > 0) {
		//Total density for the ray marching
		float density = 0.f;

		float cloud_test = 0.f;
		int zeroDensityCount = 0;
		//ray marching starting point
		vec3 position = innerIntersecPoint;

		//Total sample points 
		//float ratio = layerDist / (cloudMaxMinHeight.x - cloudMaxMinHeight.y);
		//float totalSteps = floor(baseSteps + ratio * baseSteps);
		float totalSteps = 32;
		//the length of each step
		vec3 stepLength = (layerDist / totalSteps) * rayDir;

		for (int i = 0; i < totalSteps; i++) {
			if (cloud_test > 0.f) {
				//full sample, temp as SampleDensity().
				float fullSample = SampleDensity(position);
				if (fullSample == 0) zeroDensityCount++;
				//If consecutive 0 samples have occured, that means back to large step
				if (zeroDensityCount != 6) {
					density += fullSample;
					//Calculate light
					//if (fullSample != 0) density alone light ray = SampleLight();
					//If density is full we can exit early.
					if (density > 1.f) break;
					position += stepLength;
				}
				else {
					cloud_test = 0.f;
					zeroDensityCount = 0;
				}
			}
			else {
				//cheap sample
				cloud_test = SampleDensity(position);
				if (cloud_test == 0) {
					position += stepLength;
				}
			}
		} //end of ray marching
		color += vec3(1.f, 1.f, 1.f) * density;
	}

	FragColor = vec4(color, 1);
}