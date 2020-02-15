#version 450 core

uniform mat4 modelViewMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

layout(location = 0) in vec4 position;
//layout (location = 1) in vec4 color;

out VS{
	vec4 color;
}OUT;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * modelViewMatrix * position;
	//gl_Position = position;
	//OUT.color = vec4(0.0, 0.0, 0.0, 1.0) + color;
	OUT.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}