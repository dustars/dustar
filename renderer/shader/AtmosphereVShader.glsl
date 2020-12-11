/*
	Description:
	Vertex shader for generating view ray used in atmospheic scattering.
*/


#version 450 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 position;

out vec3 view_ray;

void main() {
    //This following method of calculating ray is from the author's supervisor Rich Davison
    vec3 multiply = vec3(0, 0, 0);
    multiply.x = 1.0f/projMatrix[0][0];
    multiply.y = 1.0f/projMatrix[1][1];

    vec3 tempPos = (position * multiply) - vec3(0, 0, 1);
    
    //The viewMatrix has already been transposed set by the flag in Uniform method.
    tempPos = mat3(viewMatrix) * normalize(tempPos);

    //See comments in RenderAtmosphericScatteringAtmosphereModel() in renderer class for reference.
    view_ray = tempPos.zxy;
    
    gl_Position = vec4(position, 1.0);
}