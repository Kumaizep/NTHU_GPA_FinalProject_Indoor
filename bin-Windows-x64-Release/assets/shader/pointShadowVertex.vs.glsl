
#version 450

layout (location = 0) in vec3 aPos;

// uniform mat4 model;
uniform mat4 um4m;

void main()
{
    gl_Position = um4m * vec4(aPos, 1.0);
}