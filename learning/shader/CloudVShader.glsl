#version 450 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out VS{
	vec2 texCoord;
}OUT;

void main(void)
{
	gl_Position = vec4(position * 2.0, 1.0); //Double the quad here seems a waste of GPU resources.
	OUT.texCoord = vec2(texCoord.x, -texCoord.y);
}