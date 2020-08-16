#version 450 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out VS{
	vec2 texCoord;
	vec3 viewRay;
}OUT;

void main(void)
{
    vec3 multiply = vec3(0, 0, 0);
    multiply.x = 1.0f / projMatrix[0][0];
    multiply.y = 1.0f / projMatrix[1][1];

    vec3 tempPos = (position * multiply) - vec3(0, 0, 1);

    OUT.viewRay = mat3(viewMatrix) * normalize(tempPos);

	OUT.texCoord = vec2(texCoord.x, -texCoord.y);
	gl_Position = vec4(position, 1.f);
}