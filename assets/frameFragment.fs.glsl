#version 450

in vec2 texCoords;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D textureNoise;
layout(binding = 2) uniform sampler2D textureBloom;

uniform int timer;
uniform int testMode;
uniform int filterMode;
uniform vec2 textureSizeReciprocal;
uniform vec2 frameSize;

#define M_PI 3.1415926535897932384626433832795

const float coeffs[5] = float[5](0.0625f, 0.25f, 0.375, 0.25f, 0.0625f);
vec4 blurHDR()
{
	vec4 sum = vec4(0.0);
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
		{
			vec2 tc = texCoords + vec2(float(i - 2), float(j - 2))/frameSize;
			sum += coeffs[i] * coeffs[j] * texture(textureBloom, tc);
		}
	return sum;
}

void bloomDraw(){
    fragColor = texture(texture0, texCoords);
    fragColor += blurHDR();
}

void main()
{
    bloomDraw();
}