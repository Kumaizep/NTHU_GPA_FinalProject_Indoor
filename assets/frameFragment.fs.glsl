#version 450

in vec2 texCoords;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D textureNoise;

uniform int timer;
uniform int testMode;
uniform int filterMode;
uniform vec2 textureSizeReciprocal;
uniform vec2 frameSize;

#define M_PI 3.1415926535897932384626433832795

void filterDraw()
{

    fragColor = texture(texture0, texCoords);
}


void main()
{
    filterDraw();
}