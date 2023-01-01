#version 450

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 iv3normal;
layout(location = 2) in vec2 iv2tex_coord;
layout(location = 3) in vec3 iv3tangent;
layout(location = 4) in vec3 iv3bitangent;

uniform mat4 um4v;
uniform mat4 um4m;
uniform mat4 um4p;

out VertexData
{
    vec3 worldPosition;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec4 shadow_coord; // Light-space coordinates
} vertexData;

void main(void)
{
    gl_Position = um4p * um4v * um4m * vec4(iv3vertex, 1.0);
}