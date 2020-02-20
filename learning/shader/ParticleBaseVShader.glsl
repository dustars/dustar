#version 450 core

uniform mat4 TransformMatrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

out VS{
	vec4 color;
	vec2 texCoord;
}OUT;

void main(void)
{
	gl_Position = TransformMatrix * position;

	OUT.color = vec4(1.0, 1.0, 1.0, 1.0);
	OUT.texCoord = texCoord;
}