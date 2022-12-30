#version 450

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 color1; // world space vertex
layout(location = 2) out vec4 color2; // world space normal
layout(location = 3) out vec4 color3; // ambient color map
layout(location = 4) out vec4 color4; // diffuse color map
layout(location = 5) out vec4 color5; // specular color map
layout(location = 6) out vec4 color6; // world space tangent

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
	vec3 P; // eye space position
    vec3 T;
    vec3 B;

    vec3 TBNL;
    vec3 TBNH;
    vec3 TBNV;

    vec3 worldPosition;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
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

uniform bool blinnPhongEnabled;
uniform bool normalMappingEnabled;
uniform bool bloomEffectEnabled;
uniform bool lightMode;

uniform int textureNumber;
uniform int haveMapDiffuse;
uniform int haveMapHeight;
layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;
uniform int colorChannel0;
uniform int colorChannel1;
uniform int colorChannel2;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointlight;


vec4 blinnPhong(vec3 Kdiffuse, vec3 N, vec3 L ,vec3 H)
{
	vec3 ambient = Ka * Ia;
	vec3 diffuse = Kdiffuse * Id * max(dot(L, N), 0.0);
	vec3 specular = Ks * Is * pow(max(dot(H, N), 0.0), Ns); 

	return vec4(ambient + diffuse + specular, 1.0f);
}

vec4 CalcPointLight(vec3 Kdiffuse, vec3 N, PointLight light, vec3 P)
{
    vec3 lightDir = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 H = normalize(lightDir - normalize(P));
    return blinnPhong(Kdiffuse, N, lightDir, H) * attenuation;
}

vec4 CalcPointLightNormalMap(vec3 Kdiffuse, vec3 N, PointLight light, vec3 P)
{
    vec3 lightDir = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 TBNL = normalize(vec3(dot(lightDir, vertexData.T), dot(lightDir, vertexData.B), dot(lightDir, vertexData.N)));
    vec3 H = normalize(TBNL + vertexData.TBNV);

    return blinnPhong(Kdiffuse, N, TBNL, H) * attenuation;
}

void mixDraw(vec3 Kdiffuse, vec3 N, vec3 L, vec3 H, vec3 P, PointLight light)
{
	fragColor = vec4(0.0);
	if (!blinnPhongEnabled && !bloomEffectEnabled)
	{
		fragColor = vec4(Kdiffuse, 1.0);
	}
	else
	{
		if (blinnPhongEnabled)
		{
			fragColor += blinnPhong(Kdiffuse, N, L, H);
		}

		if (bloomEffectEnabled)
		{
			if (haveMapHeight == 1 && normalMappingEnabled)
			{
				fragColor += CalcPointLightNormalMap(Kdiffuse, N, pointlight, P);
			}
			else
			{
				fragColor += CalcPointLight(Kdiffuse, N, pointlight, P);
			}
		}
	}
}

void main()
{
	vec4 Kdiffuse = vec4(Kd, 0.0);
	vec3 N = vertexData.N;
	vec3 L = vertexData.L;
	vec3 H = vertexData.H;

	if(lightMode)
	{
		fragColor = vec4(1.0);
	}
	else if (haveMapDiffuse == 1)
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
		L = vertexData.TBNL;
		H = vertexData.TBNH;
	}
	
	mixDraw(Kdiffuse.rgb, N, L, H, vertexData.P, pointlight);

	color1 = vec4(vertexData.worldPosition, Ns);
	color2 = vec4(vertexData.normal, 0.0);
	color3 = vec4(Ka, 0.0);
	color4 = Kdiffuse;
	color5 = vec4(Ks, 0.0);
	color6 = vec4(vertexData.tangent, 0.0);
}
