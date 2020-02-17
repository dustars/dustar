#version 450 core

uniform mat4 TransformMatrix;
uniform vec3 color;

layout (location = 0) in vec4 position;

out VS{
	vec4 color;
}OUT;

void main(void)
{
	gl_Position = TransformMatrix * position;

	OUT.color = vec4(color, 1.0);
}