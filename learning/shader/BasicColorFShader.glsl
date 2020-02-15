#version 450 core

in VS{
	vec4 color;
}IN;

out vec4 color;

void main(void)
{
	color = IN.color;
}