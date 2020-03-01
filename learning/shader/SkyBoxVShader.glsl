#version 450 core

uniform mat4 ProjMatrix;
uniform mat4 ViewMatrix;

layout(location = 0) in vec3 position;

out vec3 TexCoord;

void main()
{
    //TexCoord = position;
    //gl_Position = ProjMatrix * ViewMatrix * vec4(position, 1.0f);
    vec3 multiply = vec3(0, 0, 0);
    multiply.x = 1.0f/ProjMatrix[0][0];
    multiply.y = 1.0f/ProjMatrix[1][1];

    vec3 tempPos = (position * multiply) - vec3(0, 0, 1);
    TexCoord = transpose(mat3(ViewMatrix)) * normalize(tempPos);
    gl_Position = vec4(position, 1.0);

}