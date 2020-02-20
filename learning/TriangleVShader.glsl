#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

out VS{
	vec4 color;
	vec2 texCoord;
}OUT;

void main(void) {
	gl_Position = position;
	OUT.color = color;
	OUT.texCoord = texCoord;
}