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

uniform mat4 um4v;
uniform mat4 um4p;
uniform vec2 frameSize;
uniform int gBufferMode;
uniform bool effectTestMode;
uniform bool bloomEffectEnabled;
uniform bool NPREnabled;
uniform bool SSREnabled;

vec4 blurHDRColor(vec2 tc)
{
    vec4 result = texture(texture0, tc);
    float brightness = dot(result, vec4(0.2126, 0.7152, 0.0722, 0));
    if (result.w > 90.0)
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
            vec2 tc = texCoords + vec2(float(i - 2), float(j - 2)) / frameSize;
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

void reflect()
{
    float maxDistance = 8;
    float resolution = 0.3;
    int steps = 5;
    float thickness = 0.5;
	
    vec2 texCoord = gl_FragCoord.xy / frameSize;

    vec4 uv = vec4(0.0);

    vec4 positionFrom = um4v * texture(texture1, texCoord);

    vec3 unitPositionFrom = normalize(positionFrom.xyz);
    vec3 normal = normalize((um4v * texture(texture2, texCoord)).xyz);
    vec3 pivot = normalize(2*normal + unitPositionFrom);

    vec4 positionTo = positionFrom;

    vec4 startView = vec4(positionFrom.xyz + (pivot * 0.0), 1.0);
    vec4 endView = vec4(positionFrom.xyz + (pivot * maxDistance), 1.0);

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
        positionTo = um4v * texture(texture1, uv.xy);

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
        positionTo = um4v * texture(texture1, uv.xy);

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

    uv.ba = vec2(visibility);

    color0 = uv;
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
    if (texture(texture2, texCoords).w == 4)
        reflect();
}