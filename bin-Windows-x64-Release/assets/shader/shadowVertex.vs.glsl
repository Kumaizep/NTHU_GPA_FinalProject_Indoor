#version 450

layout(location = 0) in vec3 iv3vertex;

uniform mat4 um4v;
uniform mat4 um4m;
uniform mat4 um4p;

void main(void)
{
    gl_Position = um4p * um4v * um4m * vec4(iv3vertex, 1.0);
}