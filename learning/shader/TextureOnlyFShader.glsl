#version 450 core

uniform sampler2D ourTexture;
layout(binding = 3) uniform sampler2D worleyTex;

in VS{
	vec2 texCoord;
}IN;

out vec4 FragColor;

void main()
{
	FragColor = texture(worleyTex, IN.texCoord);
}