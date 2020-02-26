#version 450 core

uniform sampler2D Texture;

in VS{
	vec4 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blendFactor;
}IN;

out vec4 FragColor;

void main(void)
{
	FragColor = mix(texture(Texture, IN.texCoord1) , texture(Texture, IN.texCoord2), IN.blendFactor);
}