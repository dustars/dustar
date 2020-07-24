#version 450 core

//uniform mat4 modelViewMatrix;
//uniform mat4 projMatrix;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out VS{
	vec2 texCoord;
	//vec3 viewRay;
}OUT;

void main(void)
{
	gl_Position = vec4(position * 2.f, 1.f);
	//OUT.viewRay = (modelViewMatrix * vec4((projMatrix * vec4(position * 2.0, 1.0)).xyz, 0.0)).xyz;
	OUT.texCoord = vec2(texCoord.x, -texCoord.y);
}