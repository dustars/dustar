#version 450 core

layout(location = 0) uniform samplerCube skybox;

in vec3 TexCoord;

out vec4 FragColor;

void main()
{
	FragColor = texture(skybox, normalize(TexCoord));
}