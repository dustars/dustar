#version 450 core

layout (binding = 0) uniform sampler2D tex;

in VS{
	vec2 texCoord;
}IN;

out vec4 FragColor;

void main()
{
	FragColor = texture(tex, IN.texCoord);
}