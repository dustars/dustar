#version 450 core

uniform mat4 TransformMatrix;

uniform vec2 TexOffset1;
uniform vec2 TexOffset2;
uniform int numOfRows;
uniform float blendFactor;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

out VS{
	vec4 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blendFactor;
}OUT;

void main(void)
{
	gl_Position = TransformMatrix * position;

	vec2 tempCoord = texCoord/numOfRows;

	OUT.texCoord1 = tempCoord + TexOffset1;
	OUT.texCoord2 = tempCoord + TexOffset2;

	OUT.blendFactor = blendFactor;
	OUT.color = vec4(1.0, 1.0, 1.0, 1.0);
}