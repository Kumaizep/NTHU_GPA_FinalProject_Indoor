#version 450

in vec2 texCoords;

layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;

layout(binding = 0) uniform sampler2D texture0; // Default
layout(binding = 1) uniform sampler2D texture1; // world space vertex, shininess
layout(binding = 2) uniform sampler2D texture2; // world space normal, renderType
layout(binding = 3) uniform sampler2D texture3; // ambient color map
layout(binding = 4) uniform sampler2D texture4; // diffuse color map
layout(binding = 5) uniform sampler2D texture5; // specular color map
layout(binding = 6) uniform sampler2D texture6; // world space tangent, depth
layout(binding = 7) uniform sampler2D texture7; // normal mapping texture

uniform vec2 frameSize;
uniform int gBufferMode;
uniform bool effectTestMode;
uniform bool bloomEffectEnabled;
uniform bool NPREnabled;
uniform bool volumetricLightEnabled;
uniform bool SSREnabled;

uniform mat4 um4p;
uniform mat4 um4v;
uniform mat4 volumetricLightsModel;
uniform vec2 lightPositionOnScreen;

vec4 blurHDRColor(vec2 tc)
{
	vec4 result = texture(texture0, tc);
    float brightness = dot(result, vec4(0.2126, 0.7152, 0.0722, 0));
    if (result.w > 90.0)
        return result;
    else
        return vec4(0.0, 0.0, 0.0, 0.0);
}

const float coeffs[11] = float[11](0.0663417, 0.0794254, 0.0913609, 0.100969, 0.107213, 0.109379, 0.107213, 0.100969, 0.0913609, 0.0794254, 0.0663417);
vec4 blurHDR()
{
    vec4 sum = vec4(0.0);
    for (int i = 0; i < 11; ++i)
        for (int j = 0; j < 11; ++j)
        {
            vec2 tc = texCoords + vec2(float(i - 5), float(j - 5)) / frameSize;
            sum += coeffs[i] * coeffs[j] * blurHDRColor(tc);
        }
    return sum;
}

vec4 blurMID()
{
	const int blurRangeHalf = 5;
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

float sobel(int sobelMat[9])
{
	const vec2 textureSizeReciprocal = 1.0 / frameSize;

	vec4 result = vec4(0.0f);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            vec2 tc = texCoords + textureSizeReciprocal * vec2(float(i - 1), float(j - 1));
            result += sobelMat[3 * i + j] * texture(texture0, tc);
        }

	return dot(result, vec4(0.2126, 0.7152, 0.0722, 0));
}

vec4 volumetricLight()
{	
	const float exposure = 0.2;
	const float decay = 0.96815;
	const float density = 0.926;
	const float weight = 0.58767;
	const int NUM_SAMPLES = 100;

	vec2 deltaTextCoord = vec2(texCoords - lightPositionOnScreen.xy);
	vec2 textCoo = texCoords;
	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;
	vec4 result = vec4(0.0); 
	
	for(int i=0; i < NUM_SAMPLES; i++)
	{
		textCoo -= deltaTextCoord;
		vec4 samples = texture(texture6, textCoo);
		
		samples *= illuminationDecay * weight;
		
		result += samples;
		
		illuminationDecay *= decay;
	}

	result *= exposure;
	return result;
}

void reflect()
{
	color0 = texture(texture0, texCoords);
	if (bloomEffectEnabled)
	{
		color0 += blurMID() * 0.9 + blurHDR() * 0.6;
	}
	if (NPREnabled)
	{
		int sobelHorizontal[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
		int sobelVertical[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
		float edge = sqrt(pow(sobel(sobelHorizontal),2) + pow(sobel(sobelVertical),2));
		if(edge > 0.5)
			color0 = vec4(0.0f,0.0f,0.0f,1.0f);
	}
    float maxDistance = 2;
    float resolution = 0.7;
    int steps = 10;
    float thickness = 0.05;

    vec4 uv = vec4(0.0);

    vec4 positionFrom = um4v * vec4(texture(texture1, texCoords).xyz, 1.0);

    vec3 unitPositionFrom = normalize(positionFrom.xyz);
    vec3 normal = normalize(mat3(um4v) * texture(texture2, texCoords).xyz);
    vec3 pivot = normalize(unitPositionFrom - 2 * dot(normal, unitPositionFrom) * normal);

    vec4 positionTo = positionFrom;

    vec4 startView = positionFrom;
    vec4 endView = positionFrom + vec4((pivot * maxDistance), 0.0);

    vec4 startFrag = startView;
    startFrag = um4p * startFrag;
    startFrag.xyz /= startFrag.w;
    startFrag.xy = startFrag.xy * 0.5 + 0.5;
    startFrag.xy *= frameSize;

    vec4 endFrag = endView;
    endFrag = um4p * endFrag;
    endFrag.xyz /= endFrag.w;
    endFrag.xy = endFrag.xy * 0.5 + 0.5;
    endFrag.xy *= frameSize;

    vec2 frag = startFrag.xy;
    uv.xy = frag / frameSize;

    float deltaX = endFrag.x - startFrag.x;
    float deltaY = endFrag.y - startFrag.y;
    float useX = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
    vec2 increment = vec2(deltaX, deltaY) / max(delta, 0.001);

    float search0 = 0;
    float search1 = 0;

    int hit0 = 0;
    int hit1 = 0;

    float viewDistance = startView.y;
    float depth = thickness;

    float i = 0;

    for (i = 0; i < int(delta); ++i)
    {
        frag += increment;
        uv.xy = frag / frameSize;
        positionTo = um4v * vec4(texture(texture1, uv.xy).xyz, 1.0);

        search1 =
            mix((frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);

        search1 = clamp(search1, 0.0, 1.0);

        viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
        depth = viewDistance - positionTo.y;

        if (depth > 0 && depth < thickness)
        {
            hit0 = 1;
            break;
        }
        else
        {
            search0 = search1;
        }
    }

    search1 = search0 + ((search1 - search0) / 2.0);

    steps *= hit0;

    for (i = 0; i < steps; ++i)
    {
        frag = mix(startFrag.xy, endFrag.xy, search1);
        uv.xy = frag / frameSize;
        positionTo = um4v * vec4(texture(texture1, uv.xy).xyz, 1.0);

        viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
        depth = viewDistance - positionTo.y;

        if (depth > 0 && depth < thickness)
        {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2);
        }
        else
        {
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2);
            search0 = temp;
        }
    }

    float visibility =
        hit1 * positionTo.w * (1 - max(dot(-unitPositionFrom, pivot), 0)) * (1 - clamp(depth / thickness, 0, 1)) * (1 - clamp(length(positionTo - positionFrom) / maxDistance, 0, 1)) * (uv.x < 0 || uv.x > 1 ? 0 : 1) * (uv.y < 0 || uv.y > 1 ? 0 : 1);

    visibility = clamp(visibility, 0, 1);
    visibility = 0.2;
    if(hit1>0 && abs(uv.x)<1&&abs(uv.y)<1)
    color0  = color0 *(1-visibility) + texture(texture0, uv.xy) * visibility;
}

void main()
{
	color0 = texture(texture0, texCoords);
    if (bloomEffectEnabled)
    {
        color0 += blurMID() * 0.9 + blurHDR() * 0.7;
    }
    if (NPREnabled)
    {
        int sobelHorizontal[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
        int sobelVertical[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
        float edge = sqrt(pow(sobel(sobelHorizontal), 2) + pow(sobel(sobelVertical), 2));
        if (edge > 0.5)
            color0 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    if (SSREnabled)
    {
    	if (texture(texture2, texCoords).w == 4)
        	reflect();
    }
    

	if (volumetricLightEnabled)
		color0 += volumetricLight() * 0.4;
}