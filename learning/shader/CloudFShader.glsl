#version 450 core

#define MAX_STEPS 50
#define MAX_DIST 2000.
#define SURF_EPSILON .01

#define NUM_SPHERES 4

// Scene FBO
layout(binding = 0) uniform sampler2D renderFBO;
layout(binding = 1) uniform sampler2D renderDepth;

uniform vec3 cameraPos;
uniform vec2 resolution;
uniform mat4 rotationMatrix;

vec4[NUM_SPHERES] spheres = { {0, 0, -6, 1},
							{0, 0, 6, 1},
							{6, 0, 0, 1},
							{-6, 0, 0, 1} };

in VS{
	vec2 texCoord;
}IN;

out vec4 FragColor;

float GetSDF(vec3 pos)
{
	float nearestDis = 0;

	for (int i = 0; i < NUM_SPHERES; i++)
	{
		float tempDis = length(pos - spheres[i].xyz) - spheres[i].w;
		if (nearestDis > tempDis || nearestDis == 0)
		{
			nearestDis = tempDis;
		}
	}

	//pos.y + 10 is the hard-coded floor
	//return min(nearestDis, pos.y + 10);
	return nearestDis;
}

float RayMarch(vec3 rayPos, vec3 rayDir)
{
	float dis = 0;
	for (int i = 0; i < MAX_STEPS; i++)
	{
		vec3 pos = rayPos + rayDir * dis;
		float distance = GetSDF(pos);
		dis += distance;
		if (distance > MAX_DIST) {
			return 0;
		}
		if(distance < SURF_EPSILON) break;
	}
	return dis;
}

vec3 NormalCalculation(vec3 pos)
{
	float d = GetSDF(pos);
	vec2 e = vec2(.01, 0);

	vec3 n = d - vec3(
		GetSDF(pos - e.xyy),
		GetSDF(pos - e.yxy),
		GetSDF(pos - e.yyx));

	return normalize(n);
}

float LightCalculation(vec3 pos)
{
	//hard-coded light
	vec3 lightPos = vec3(0, 10, 0);
	vec3 lDir = normalize(lightPos - pos);
	vec3 normal = NormalCalculation(pos);

	float cos = clamp(dot(lDir, normal), 0., 1.);
	//float shadowDis = RayMarch(pos+ normal *SURF_EPSILON*2., lDir);
	//if (shadowDis < length(lightPos - pos)) cos *= .1;
	return cos;
}

void main(void)
{
	vec3 color = texture(renderFBO, IN.texCoord).xyz;
	float depth = texture(renderDepth, IN.texCoord).x;

	//Inverse transformation on non-linear depth value to get linear z-value
	depth = depth * 2.f - 1.f;
	depth = 30000.f / (15001.f - depth * 14999.f); //harded-code near and far plane, see https://learnopengl.com/Advanced-OpenGL/Depth-testing

	vec2 uv = (gl_FragCoord.xy - .5 * resolution) / resolution.y;

	vec3 rayPos = cameraPos;
	vec3 rayDir = normalize((( rotationMatrix * vec4(uv, -1, 1))).xyz);

	float dis = RayMarch(rayPos, rayDir);
	if ( dis > 0 && dis < depth)
	{
		color = vec3(dis);
		float cos = LightCalculation(rayPos + dis * rayDir);
		color = vec3(cos);
	}
	
	FragColor = vec4(color, 1);
}