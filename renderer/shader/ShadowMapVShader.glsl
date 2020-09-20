#version 450 core

uniform mat4 lightMatrix;

layout(location = 0) in vec3 position;

void main(void) {
	gl_Position = lightMatrix * vec4(position, 1.0);
}