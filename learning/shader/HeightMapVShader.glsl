#version 450 core

uniform int HaveColor = 0;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out VS{
	vec3 uFUnlit;
	vec3 uWarmColor;
	vec2 texCoord;
	vec3 vPos;
	vec3 vNormal;
}OUT;

void main(void) {
	
	OUT.vNormal = (ModelMatrix * vec4(normal, 1.0f)).xyz;

	vec4 temp = ModelMatrix * vec4(position, 1.0f);
	OUT.vPos = temp.xyz;

	if (HaveColor == 0) {
		OUT.uFUnlit = vec3(1, 1, 1);
		OUT.uWarmColor = vec3(1, 1, 1);
	} else{
		OUT.uFUnlit = ((0, 0, 0.55) + 0.25 * color) * 0.5;
		OUT.uWarmColor = (0.3, 0.3, 0) + 0.25 * color;
	}
	
	OUT.texCoord = texCoord;

	gl_Position = ProjMatrix * ViewMatrix * temp;
}