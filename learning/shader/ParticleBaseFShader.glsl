#version 450 core

uniform sampler2D Texture;

in VS{
	vec4 color;
	vec2 texCoord;
}IN;

out vec4 FragColor;

void main(void)
{
	FragColor = texture(Texture, IN.texCoord);
}