#version 450

in vec2 texCoords;
layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;

layout(binding = 0) uniform sampler2D texture0; 		// Default
layout(binding = 1) uniform sampler2D texture1; 		// world space vertex
layout(binding = 2) uniform sampler2D texture2; 		// world space normal
layout(binding = 3) uniform sampler2D texture3; 		// ambient color map
layout(binding = 4) uniform sampler2D texture4; 		// diffuse color map
layout(binding = 5) uniform sampler2D texture5; 		// specular color map
layout(binding = 6) uniform sampler2D texture6; 		// world space tangent
layout(binding = 7) uniform sampler2D texture7; 		// normal mapping texture
layout(binding = 8) uniform sampler2DShadow texture8; 	// directional shadow map
layout(binding = 9) uniform samplerCube texture9;		// point light shadow map


uniform vec2 frameSize;
uniform int gBufferMode;
uniform bool effectTestMode;
uniform bool blinnPhongEnabled;
uniform bool directionalShadowEnabled;
uniform bool pointShadowEnabled;
uniform bool normalMappingEnabled;
uniform bool bloomEffectEnabled;

uniform mat4 um4v;
uniform mat4 shadow_sbpv;
uniform vec3 dirtectionalShadowPosition;

const vec3 Ia = vec3(0.1, 0.1, 0.1);
const vec3 Id = vec3(0.7, 0.7, 0.7);
const vec3 Is = vec3(0.2, 0.2, 0.2);

struct PointLight {
    vec3 position;
    vec3 worldPosition;
    
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointlight;

vec4 blinnPhong(vec3 N, vec3 L ,vec3 H, float shadow)
{
	vec3 Ka = texture(texture3, texCoords).xyz;
	vec3 Kd = texture(texture4, texCoords).xyz;
	vec3 Ks = texture(texture5, texCoords).xyz;
	float Ns = texture(texture1, texCoords).w;
	// ambient
	vec3 ambient = Ka * Ia;
	// diffuse
	vec3 diffuse = Kd * Id * max(dot(L, N), 0.0);
	// specular
	vec3 specular = Ks * Is * pow(max(dot(H, N), 0.0), Ns); 

	return vec4(ambient + shadow*(diffuse + specular) , 1.0f);
}

float ShadowCalculation()
{
    // get vector between fragment position and light position
    vec3 fragToLight = texture(texture1, texCoords).xyz - pointlight.worldPosition;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(texture9, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= 10;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.02; // we use a much larger bias since depth is now in [near_plane, far_plane] range

    if (currentDepth - bias > closestDepth)
		return 0.0f;
	return 1.0f;     
}

vec4 CalcPointLight(vec3 N, PointLight light, vec3 P)
{
    vec3 lightDir     = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 H = normalize(lightDir - normalize(P));
	if(pointShadowEnabled)
    	return blinnPhong(N, lightDir, H, ShadowCalculation()) * attenuation;
    return blinnPhong(N, lightDir, H, 1.0f) * attenuation;
}

vec4 CalcPointLightNormalMap(vec3 N, PointLight light, vec3 P, vec3 T, vec3 B, vec3 TBNV, vec3 originNormal)
{
    vec3 lightDir 	  = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 TBNL = normalize(vec3(dot(lightDir, T), dot(lightDir, B), dot(lightDir, originNormal)));
    vec3 H = normalize(TBNL + TBNV);

	if(pointShadowEnabled)
    	return blinnPhong(N, TBNL, H, ShadowCalculation()) * attenuation;
    return blinnPhong(N, TBNL, H, 1.0f) * attenuation;
}


void defaultDraw()
{
	vec3 vertex = texture(texture1, texCoords).xyz;
	vec3 normal = texture(texture2, texCoords).xyz;
	vec3 tangent = texture(texture6, texCoords).xyz;

	vec4 P = um4v * vec4(vertex, 1.0);
	vec4 LW = um4v * vec4(-2.845, 2.028, -1.293, 1.0);
	if (directionalShadowEnabled)
		LW = um4v * vec4(dirtectionalShadowPosition, 1.0);
	// Eye space to tangent space TBN
	vec3 N = normalize(mat3(um4v) * normal);
	vec3 L = normalize(LW.xyz - P.xyz);
	vec3 V = normalize(-P.xyz);
	vec3 H = normalize(L + V);
	vec3 T = vec3(0.0);
	vec3 B = vec3(0.0);

	float renderType = texture(texture2, texCoords).w;
	if (renderType == 1)
	{
		// Modify parameter for the case normal mapping enabled
		T = normalize(mat3(um4v) * tangent);
		B = cross(N, T);
		L = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
		V = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));
		H = normalize(L + V);
		N = texture(texture7, texCoords).xyz;
	}

	if (!blinnPhongEnabled && !bloomEffectEnabled)
	{
		// Default render: Kd and Kd_map
		color0 = vec4(texture(texture4, texCoords).xyz, 1.0);
	}
	else
	{
		color0 = vec4(0.0);
		if (blinnPhongEnabled)
		{
			// Blinn Phong render
			vec4 shadow_coord = shadow_sbpv * vec4(texture(texture1, texCoords).xyz,1);
			float shadow = textureProj(texture8, shadow_coord+ vec4(0,0,-0.02,0));
			if(directionalShadowEnabled)
				color0 += blinnPhong(N, L, H, shadow);
			else
				color0 += blinnPhong(N, L, H, 1.0f);
		}
		if (bloomEffectEnabled)
		{
			// Blinn Phong render
			if (renderType == 0)
			{
					color0 += CalcPointLight(N, pointlight, P.xyz);
			}
			else if (renderType == 1)
			{
				vec3 originNormal = normalize(mat3(um4v) * normal);
					color0 += CalcPointLightNormalMap(N, pointlight, P.xyz, T, B, V, originNormal);
			}
			else if (renderType == 2)
			{
				color0 = vec4(1.0, 1.0, 1.0, 100.0);
			}
		}
	}
}

void gBufferDraw()
{
	switch (gBufferMode) {
		case 0:
			defaultDraw();
			break;
		case 1:
			color0 = texture(texture1, texCoords);
			color0 = vec4(normalize(color0.xyz) * 0.5 + 0.5, 1.0);
			break;
		case 2:
			color0 = texture(texture2, texCoords);
			color0 = vec4(normalize(color0.xyz) * 0.5 + 0.5, 1.0);
			break;
		case 3:
			color0 = texture(texture3, texCoords);
			break;
		case 4:
			color0 = texture(texture4, texCoords);
			break;
		case 5:
			color0 = texture(texture5, texCoords);
			break;
		case 6:
			color0 = texture(texture0, texCoords);
			break;
		default:
			color0 = vec4(1.0);
			break;
	}
}

void main()
{
	gBufferDraw();
}