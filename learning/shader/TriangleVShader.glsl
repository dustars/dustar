#version 450 core

uniform int HaveColor = 0;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

out VS{
	vec4 color;
	vec2 texCoord;
}OUT;

void main(void) {
	gl_Position = ProjMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0f);
	if (HaveColor > 0) {
		OUT.color = vec4(color, 1.0f);
	}
	else {
		OUT.color = vec4(0.3f, 0.6f, 0.9f, 1.0f);
	}
	OUT.texCoord = texCoord;
}