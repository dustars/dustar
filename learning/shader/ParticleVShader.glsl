#version 450 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout (location = 0) in vec4 position;

out VS{
	vec4 color;
}OUT;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * position;

	OUT.color = vec4(1.0);
}