#version 450

layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1; // world space vertex, shininess
layout(location = 2) out vec4 color2; // world space normal, renderType
layout(location = 3) out vec4 color3; // ambient color map
layout(location = 4) out vec4 color4; // diffuse color map
layout(location = 5) out vec4 color5; // specular color map
layout(location = 6) out vec4 color6; // world space tangent, depth
layout(location = 7) out vec4 color7; // normal mapping texture

in VertexData
{
    vec3 worldPosition;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
} vertexData;

const vec3 Ia = vec3(0.1, 0.1, 0.1);
const vec3 Id = vec3(0.7, 0.7, 0.7);
const vec3 Is = vec3(0.2, 0.2, 0.2);

uniform mat4 um4m;
uniform mat4 um4v;
uniform mat4 um4p;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ke;
uniform float Ni;
uniform float d;
uniform float Ns;
uniform int illum;

uniform bool normalMappingEnabled;
uniform bool lightMode;

uniform int haveMapDiffuse;
uniform int haveMapHeight;
layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;

uniform int colorChannel0;
uniform int colorChannel1;
uniform int colorChannel2;

float linearizeDepth()
{
    float near = 0.1;
    float far = 10.0;
    float z = gl_FragCoord.z * 2.0 - 1.0; // Back to NDC
    return (2.0 * near) / (far + near - z * (far - near));
}

void deferredDraw()
{
    vec3 N = normalize(mat3(um4v * um4m) * vertexData.normal);
	vec4 Kdiffuse = vec4(Kd, 0.0);
	float renderType = 0;

	if (lightMode)
	{
		renderType = 2;
	}
	else
	{
		if (haveMapDiffuse == 1)
		{
			if (colorChannel0 > 1)
			{
				Kdiffuse = texture(texture0, vertexData.texcoord);
			}
			else
			{
				Kdiffuse = texture(texture0, vertexData.texcoord).rrra;
			}

			if(Kdiffuse.a < 0.5)
			{
				discard;
			}
		}
		else if (haveMapHeight == 1 && normalMappingEnabled)
		{
			N = normalize(texture(texture0, vertexData.texcoord).rgb * 2.0 - vec3(1.0));
			renderType = 1;
		}
	}

	color1 = vec4(vertexData.worldPosition, Ns);
	color2 = vec4(mat3(um4m) * vertexData.normal, renderType);
	color3 = vec4(Ka, 0.0);
	color4 = Kdiffuse;
	color5 = vec4(Ks, 0.0);
	color6 = vec4(mat3(um4m) * vertexData.tangent, linearizeDepth());
	color7 = vec4(N, 0.0);
}

void main()
{
	// Deferred rendering enabled
	deferredDraw();
}
