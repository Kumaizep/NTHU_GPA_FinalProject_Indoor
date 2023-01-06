#version 450

in vec2 texCoords;

layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;


layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;
layout(binding = 3) uniform sampler2D texture3;
layout(binding = 4) uniform sampler2D texture4;
layout(binding = 5) uniform sampler2D texture5;
layout(binding = 6) uniform sampler2D texture6;
layout(binding = 7) uniform sampler2D texture7;

uniform vec2 frameSize;
uniform int gBufferMode;
uniform bool effectTestMode;
uniform bool bloomEffectEnabled;

vec4 blurHDRColor(vec2 tc)
{
	vec4 result = texture(texture0, tc);
	float brightness = dot(result, vec4(0.2126, 0.7152, 0.0722,0));
	if(result.w > 90.0)
		return result;
	else
		return vec4(0.0, 0.0, 0.0, 0.0);
}

const float coeffs[5] = float[5](0.0625f, 0.25f, 0.375, 0.25f, 0.0625f);
vec4 blurHDR()
{
	vec4 sum = vec4(0.0);
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
		{
			vec2 tc = texCoords + vec2(float(i - 2), float(j - 2))/frameSize;
			sum += coeffs[i] * coeffs[j] * blurHDRColor(tc);
		}
	return sum;
}

vec4 blurMID()
{
	const int blurRangeHalf = 3;
    const int blurRange = 2 * blurRangeHalf + 1;
    const vec2 textureSizeReciprocal = 1.0 / frameSize;

    vec4 blurColor = vec4(0.0f);
    for (int i = 0; i < blurRange; ++i)
    {
        for (int j = 0; j < blurRange; ++j)
        {
            vec2 tc = texCoords + textureSizeReciprocal * vec2(float(i - blurRangeHalf), float(j - blurRangeHalf));
            blurColor += blurHDRColor(tc) / blurRange / blurRange;
        }
    }

    return blurColor;
}

void draw(){
	color0 = texture(texture0, texCoords);
	if (bloomEffectEnabled)
	{
		color0 += blurMID() * 0.9 + blurHDR() * 0.7;
	}
}

void main()
{
	draw();
}