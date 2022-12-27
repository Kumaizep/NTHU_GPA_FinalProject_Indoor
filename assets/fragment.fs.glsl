#version 460

layout(location = 0) out vec4 fragColor;

in VertexData
{
	vec3 N; // eye space normal
	vec3 L; // eye space light vector
	vec3 H; // eye space halfway vector
	vec3 normal;
	vec2 texcoord;
	vec3 TBNL;
    vec3 TBNH;
} vertexData;

const vec3 Ia = vec3(0.1, 0.1, 0.1);
const vec3 Id = vec3(0.7, 0.7, 0.7);
const vec3 Is = vec3(0.2, 0.2, 0.2);

uniform mat4 um4mv;
uniform mat4 um4p;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ke;
uniform float Ni;
uniform float d;
uniform float Ns;
uniform int illum;

uniform int outputMode;
uniform bool normalMappingEnabled;

uniform int textureNumber;
uniform int haveMapDiffuse;
uniform int haveMapHeight;
layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;
uniform int colorChannel0;
uniform int colorChannel1;
uniform int colorChannel2;

void blinnPhong(vec3 Kdiffuse, vec3 N, vec3 L ,vec3 H)
{
	// ambient
	vec3 ambient = Ka * Ia;
	// diffuse
	vec3 diffuse = Kdiffuse * Id * max(dot(L, N), 0.0);
	// specular
	vec3 specular = Ks * Is * pow(max(dot(H, N), 0.0), Ns); 

	fragColor = vec4(ambient + diffuse + specular, 1.0f);
}

void main()
{
	if (outputMode == 0)
	{
		if (haveMapDiffuse == 1)
		{
			vec4 Kdiffuse;
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
			else
			{
				blinnPhong(Kdiffuse.rgb, vertexData.N, vertexData.L, vertexData.H);
			}
		}
		else if (haveMapHeight == 1 && normalMappingEnabled)
		{
			vec3 N = normalize(texture(texture0, vertexData.texcoord).rgb * 2.0 - vec3(1.0));
			blinnPhong(Kd, N, vertexData.TBNL, vertexData.TBNH);
		}
		else
		{
			blinnPhong(Kd, vertexData.N, vertexData.L, vertexData.H);
		}
	}
	else
	{
		fragColor = vec4(vertexData.normal, 1.0);
	}
}
