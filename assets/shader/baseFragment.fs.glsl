#version 450

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 color1; // world space vertex
layout(location = 2) out vec4 color2; // world space normal
layout(location = 3) out vec4 color3; // ambient color map
layout(location = 4) out vec4 color4; // diffuse color map
layout(location = 5) out vec4 color5; // specular color map
layout(location = 6) out vec4 color6; // world space tangent
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

uniform bool blinnPhongEnabled;
uniform bool normalMappingEnabled;
uniform bool bloomEffectEnabled;
uniform bool lightMode;
uniform int gBufferMode;

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
    vec3 lightDir     = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 H = normalize(lightDir - normalize(P));
    return blinnPhong(Kdiffuse, N, lightDir, H) * attenuation;
}

vec4 CalcPointLightNormalMap(vec3 Kdiffuse, vec3 N, PointLight light, vec3 P, vec3 T, vec3 B, vec3 TBNV, vec3 originNormal)
{
    vec3 lightDir     = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 TBNL = normalize(vec3(dot(lightDir, T), dot(lightDir, B), dot(lightDir, originNormal)));
    vec3 H = normalize(TBNL + TBNV);

    return blinnPhong(Kdiffuse, N, TBNL, H) * attenuation;
}

void mixDraw(vec3 Kdiffuse, vec3 N, vec3 L, vec3 H, vec3 P, vec3 T, vec3 B, vec3 TBNV, vec3 originNormal)
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
				fragColor += CalcPointLightNormalMap(Kdiffuse, N, pointlight, P, T, B, TBNV, originNormal);
			}
			else
			{
				fragColor += CalcPointLight(Kdiffuse, N, pointlight, P);
			}
		}
	}
}

void forwardDraw()
{
	vec4 P = um4v * vec4(vertexData.worldPosition, 1.0);
    vec4 LW = um4v * vec4(-2.845, 2.028, -1.293, 1.0);
    // Eye space to tangent space TBN
    vec3 T = normalize(mat3(um4v * um4m) * vertexData.tangent);
    vec3 N = normalize(mat3(um4v * um4m) * vertexData.normal);
    vec3 B = cross(N, T);
    vec3 L = normalize(LW.xyz - P.xyz);
    vec3 V = normalize(-P.xyz);
    vec3 H = normalize(L + V);
    vec3 originNormal = N;

    vec3 TBNL = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
    vec3 TBNV = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));
    vec3 TBNH = normalize(TBNL + TBNV);

	vec4 Kdiffuse = vec4(Kd, 0.0);
	float renderType = 0;

	if (lightMode)
	{
		fragColor = vec4(1.0);
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
			L = TBNL;
			H = TBNH;
			renderType = 1;
		}
		mixDraw(Kdiffuse.rgb, N, L, H, P.xyz, T, B, TBNV, originNormal);
	}
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
	color6 = vec4(mat3(um4m) * vertexData.tangent, 0.0);
	color7 = vec4(N, 0.0);
}

void main()
{
	if (gBufferMode == 6)
	{
		// Deferred rendering disabled
		forwardDraw();
	}
	else
	{
		// Deferred rendering enabled
		deferredDraw();
	}
}
