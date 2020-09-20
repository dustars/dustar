#version 450 core

uniform int HaveColor = 0;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;
uniform mat4 lightMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out VS {
	vec3 color;
	vec2 texCoord;
	vec3 vPos;
	vec3 vNormal;
	vec4 lightSpacePos;
}OUT;

void main(void) {

	vec4 temp = ModelMatrix * vec4(position, 1.0);

	OUT.color = color;
	OUT.texCoord = texCoord;
	OUT.vPos = temp.xyz;
	mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
	OUT.vNormal = normalize(normalMatrix * normalize(normal));
	OUT.lightSpacePos = lightMatrix * vec4(position, 1.0);

	gl_Position = ProjMatrix * ViewMatrix * temp;
}