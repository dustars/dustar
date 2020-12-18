#version 450 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out VS{
	vec2 texCoord;
}OUT;

void main(void) {
	gl_Position = vec4(position, 1.0f);
	OUT.texCoord = texCoord;
}