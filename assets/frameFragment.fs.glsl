#version 450

in vec2 texCoords;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1; // world space vertex
layout(binding = 2) uniform sampler2D texture2; // world space normal
layout(binding = 3) uniform sampler2D texture3; // ambient color map
layout(binding = 4) uniform sampler2D texture4; // diffuse color map
layout(binding = 5) uniform sampler2D texture5; // specular color map
layout(binding = 6) uniform sampler2D texture6; // world space tangent
layout(binding = 7) uniform sampler2D texture7; // Reserve for bloom


uniform vec2 frameSize;
uniform int gBufferMode;
uniform bool effectTestMode;
uniform bool effectTestMode2;
uniform bool bloomEffectEnabled;

uniform mat4 um4v;

const vec3 Ia = vec3(0.1, 0.1, 0.1);
const vec3 Id = vec3(0.7, 0.7, 0.7);
const vec3 Is = vec3(0.2, 0.2, 0.2);


#define M_PI 3.1415926535897932384626433832795

vec4 blurHDRColor(vec2 tc)
{
    vec4 result = texture(texture0, tc);
    float brightness = dot(result, vec4(0.2126, 0.7152, 0.0722,0));
    if(brightness > 0.9)
        return result;
    else
        return vec4(0.0, 0.0, 0.0, 1.0);
}

const float coeffs[5] = float[5](0.0625f, 0.25f, 0.375, 0.25f, 0.0625f);
vec4 blurHDR()
{
	vec4 sum = vec4(0.0);
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
		{
			vec2 tc = texCoords + vec2(float(i - 2), float(j - 2))/frameSize;
            // TAG: Merge bloom frame shader
            // ================================================================
            sum += coeffs[i] * coeffs[j] * blurHDRColor(tc);
			// sum += coeffs[i] * coeffs[j] * texture(texture7, tc);
            // ================================================================
		}
	return sum;
}

void bloomDraw(){
    fragColor = texture(texture0, texCoords);
	if (effectTestMode)
		fragColor += blurHDR();
}

void blinnPhongColor(vec3 N, vec3 L ,vec3 H, float Ns)
{
    vec3 Ka = texture(texture3, texCoords).xyz;
    vec3 Kd = texture(texture4, texCoords).xyz;
    vec3 Ks = texture(texture5, texCoords).xyz;
    // ambient
    vec3 ambient = Ka * Ia;
    // diffuse
    vec3 diffuse = Kd * Id * max(dot(L, N), 0.0);
    // specular
    vec3 specular = Ks * Is * pow(max(dot(H, N), 0.0), Ns); 

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}

void blinnPhong()
{
    vec3 vertex = texture(texture1, texCoords).xyz;
    vec3 normal = texture(texture2, texCoords).xyz;
    vec3 tangent = texture(texture6, texCoords).xyz;

    vec4 P = um4v * vec4(vertex, 1.0);
    vec4 LW = um4v * vec4(-2.845, 2.028, -1.293, 1.0);
    // Eye space to tangent space TBN
    vec3 T = normalize(mat3(um4v) * tangent);
    vec3 N = normalize(mat3(um4v) * normal);
    vec3 B = cross(N, T);
    vec3 L = LW.xyz - P.xyz;
    vec3 V = -P.xyz;

    vec3 TBNL = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
    vec3 TBNV = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));
    vec3 TBNH = normalize(TBNL + TBNV);

    L = normalize(L);
    vec3 H = normalize(L + normalize(V));

    float Ns = texture(texture1, texCoords).w;
    blinnPhongColor(N, L, H, Ns);
}

void gBufferDraw()
{
    switch (gBufferMode) {
        case 0:
            bloomDraw();
            break;
        case 1:
            fragColor = texture(texture1, texCoords);
            fragColor = vec4(normalize(fragColor.xyz) * 0.5 + 0.5, 1.0);
            break;
        case 2:
            fragColor = texture(texture2, texCoords);
            fragColor = vec4(normalize(fragColor.xyz) * 0.5 + 0.5, 1.0);
            break;
        case 3:
            fragColor = texture(texture3, texCoords);
            break;
        case 4:
            fragColor = texture(texture4, texCoords);
            break;
        case 5:
            fragColor = texture(texture5, texCoords);
            break;
        case 6:
            blinnPhong();
            break;
        default:
            fragColor = vec4(1.0);
            break;
    }
}

void main()
{
    gBufferDraw();
}