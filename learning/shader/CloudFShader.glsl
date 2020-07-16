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

uniform vec3 cameraPos;
uniform vec2 resolution;
uniform mat4 rotationMatrix;

//Cloud layer is 2km above the origin, and the Vanishing point is 10km,
//which gives a sphere located -23880m below the origin with a 25880 radius.
vec4 cloudSphere = vec4(0, -23880, 0, 25880);

vec3 cloudBoxPos = vec3(0, 0, 0);
vec3 cloudBoxSize = vec3(0, 0, 0);

float CloudScale = 1.f;

vec3 lightRay = vec3(2.f, 2.f, -1.f);
float eccentricityG = 0.2f;


in VS{
	vec2 texCoord;
}IN;

out vec4 FragColor;

//vec4[NUM_SPHERES] spheres = { {0, 0, -6, 1},
//							{0, 0, 6, 1},
//							{6, 0, 0, 1},
//							{-6, 0, 0, 1} };
//
//float GetSDFSphere(vec3 pos)
//{
//	float nearestDis = 0;
//
//	for (int i = 0; i < NUM_SPHERES; i++)
//	{
//		float tempDis = length(pos - spheres[i].xyz) - spheres[i].w;
//		if (nearestDis > tempDis || nearestDis == 0)
//		{
//			nearestDis = tempDis;
//		}
//	}
//
//	//pos.y + 10 is the hard-coded floor
//	//return min(nearestDis, pos.y + 10);
//	return nearestDis;
//}


float RayShpereMarching(vec3 rayPos, vec3 rayDir)
{
	float dis = 0;
	for (int i = 0; i < MAX_STEPS; i++) {
		vec3 pos = rayPos + rayDir * dis;

		float distance = abs(length(pos - cloudSphere.xyz) - cloudSphere.w);

		dis += distance;

		if (distance > MAX_DIST) return 0;
		if (abs(distance) < SURF_EPSILON) break;
	}
	if ((rayPos + rayDir * dis).y < 0) return 0;
	return dis;
}

float RayBoxMarching(vec3 rayPos, vec3 rayDir)
{
	float dis = 0;
	//Calculate distance to box;
	for (int i = 0; i < MAX_STEPS; i++) {
		vec3 pos = rayPos + rayDir * dis;

		vec3 temp = abs(pos - cloudBoxPos) - cloudBoxSize;
		float distance = length(max(temp, 0)) + min(max(max(temp.x, temp.y), temp.z), 0);

		dis += distance;

		if (distance > MAX_DIST) return 0;
		if (distance < SURF_EPSILON) break;
	}
	return dis;
}

float SampleDensity(vec3 pos)
{
	vec3 uvw = pos * CloudScale * 0.0001; // how to get uvw?
	float density = texture(cloudBaseTexture, uvw).x; //Only got red channel, is .x needed?
	return max(0, density - 0.6)*3.f;
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
	depth = 30000.f / (15001.f - depth * 14999.f); //harded-code near and far plane, see https://learnopengl.com/Advanced-OpenGL/Depth-testing

	//Calculate ray position and direction
	vec2 uv = (gl_FragCoord.xy - .5 * resolution) / resolution;
	vec3 rayPos = cameraPos;
	vec3 rayDir = normalize(((rotationMatrix * vec4(normalize(vec3(uv, -1)), 1))).xyz);

	//Ray marching
	//vec2 rayInfo = BoxRayMarching(rayPos, rayDir);
	float rayDist = RayShpereMarching(rayPos, rayDir);
	float rayInsideBox = 128.f;
	
	if (rayDist > 0  && rayDist < depth) {
		float disTravelled = 0;
		float stepSize = rayInsideBox / 10;

		float density = 0;
		while (disTravelled < rayInsideBox) {
			density += SampleDensity(rayPos + rayDir * (rayDist + disTravelled));

			disTravelled += stepSize;
		}
		
		color *= exp(-density/10);
		color += vec3(0.95f, 0.95f, 0.9f) * vec3(lightCalculation(density, rayDir));
	}

	FragColor = vec4(color, 1);
}