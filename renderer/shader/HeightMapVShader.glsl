#version 450 core

uniform int HaveColor = 0;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color; //unused
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out VS
{
	vec2 texCoord;
	vec3 vPos;
	vec3 vNormal;
}OUT;

void main(void)
{

	vec4 temp = ModelMatrix * vec4(position, 1.0f);

	OUT.texCoord = texCoord;
	OUT.vPos = temp.xyz;
	OUT.vNormal = (ModelMatrix * vec4(normal, 1.0f)).xyz;

	gl_Position = ProjMatrix * ViewMatrix * temp;
}