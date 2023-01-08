#version 450

in vec2 texCoords;
layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;
layout(location = 4) out vec4 color4;
layout(location = 5) out vec4 color5;
layout(location = 6) out vec4 color6; // VL projection
layout(location = 7) out vec4 color7;

layout(binding = 0) uniform sampler2D texture0; 		// Default
layout(binding = 1) uniform sampler2D texture1; 		// world space vertex, shininess
layout(binding = 2) uniform sampler2D texture2; 		// world space normal, renderType
layout(binding = 3) uniform sampler2D texture3; 		// ambient color map
layout(binding = 4) uniform sampler2D texture4; 		// diffuse color map
layout(binding = 5) uniform sampler2D texture5; 		// specular color map
layout(binding = 6) uniform sampler2D texture6; 		// world space tangent, depth
layout(binding = 7) uniform sampler2D texture7; 		// normal mapping texture
layout(binding = 8) uniform sampler2DShadow texture8; 	// shadow map
layout(binding = 9) uniform samplerCube texture9;		// point light shadow map 
layout(binding = 10) uniform sampler2D texture10;		// LTC1
layout(binding = 11) uniform sampler2D texture11;		// LTC2

layout(std140) uniform SSAOKernals
{
	vec3 val[32];
};

uniform vec2 frameSize;
uniform int gBufferMode;
uniform bool effectTestMode;
uniform bool blinnPhongEnabled;
uniform bool directionalShadowEnabled;
uniform bool pointShadowEnabled;
uniform bool normalMappingEnabled;
uniform bool bloomEffectEnabled;
uniform bool SSAOEnabled;
uniform bool NPREnabled;
uniform bool areaLightEnabled;

uniform mat4 um4v;
uniform mat4 um4p;
uniform mat4 shadow_sbpv;
uniform vec3 dirtectionalShadowPosition;

uniform vec3 sampleKernel[64];

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

// DEV-AreaLight
struct Light
{
    float intensity;
	float roughness;
    vec3 color;
    vec3 points[4];
	vec3 translate;
	mat4 rotateMat;
};
uniform Light areaLight;
uniform vec3 viewPosition;
const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0) / LUT_SIZE;
const float LUT_BIAS  = 0.5 / LUT_SIZE;



vec4 blinnPhong(vec3 N, vec3 L ,vec3 H, float shadow, vec3 SSAO)
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
	
	return vec4(ambient * SSAO + shadow * (diffuse + specular) , 1.0f);
}

vec4 blinnNPR(vec3 N, vec3 L ,vec3 H, float shadow, vec3 SSAO)
{
	if(NPREnabled)
	{
		float light = ceil(max(dot(N, L) * 3,0)) / 3;
		return texture(texture4, texCoords) * light * shadow;
	}
	else
		return blinnPhong(N, L, H, shadow, SSAO);
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

vec4 CalcPointLight(vec3 N, PointLight light, vec3 P, vec3 SSAO)
{
    vec3 lightDir     = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 H = normalize(lightDir - normalize(P));
    if(pointShadowEnabled)
    	return blinnNPR(N, lightDir, H, ShadowCalculation(), SSAO) * attenuation;
    return blinnNPR(N, lightDir, H, 1.0f, SSAO) * attenuation;
}

vec4 CalcPointLightNormalMap(vec3 N, PointLight light, vec3 P, vec3 T, vec3 B, vec3 TBNV, 
	vec3 originNormal, vec3 SSAO)
{
    vec3 lightDir 	  = normalize(light.position - P);
    float distance    = length(light.position - P);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
	
    vec3 TBNL = normalize(vec3(dot(lightDir, T), dot(lightDir, B), dot(lightDir, originNormal)));
    vec3 H = normalize(TBNL + TBNV);

    if(pointShadowEnabled)
    	return blinnNPR(N, TBNL, H, ShadowCalculation(), SSAO) * attenuation;
    return blinnNPR(N, TBNL, H, 1.0f, SSAO) * attenuation;
}

vec3 blurSSAO() {
	if (!SSAOEnabled)
		return vec3(1.0);

    vec2 texelSize = 1.0 / vec2(textureSize(texture0, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(texture0, texCoords + offset).r;
        }
    }
    return vec3(result / 16.0);
} 

vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

float IntegrateEdge(vec3 v1, vec3 v2)
{
    return IntegrateEdgeVec(v1, v2).z;
}

vec3 PowVec3(vec3 v, float p)
{
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

const float gamma = 2.2;
vec3 ToLinear(vec3 v) { return PowVec3(v, gamma); }
vec3 ToSRGB(vec3 v)   { return PowVec3(v, 1.0 / gamma); }

vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4])
{
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = Minv * transpose(mat3(T1, T2, N));

    // polygon (allocate 4 vertices for clipping)
    vec3 L[4];
    // transform polygon from LTC back to origin Do (cosine weighted)
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);
    L[3] = Minv * (points[3] - P);

    // use tabulated horizon-clipped sphere
    // check if the shading point is behind the light
    vec3 dir = points[0] - P; // LTC space
    vec3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0);

    // cos weighted space
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    // integrate
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    // form factor of the polygon in direction vsum
    float len = length(vsum);

    float z = vsum.z/len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z*0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(texture11, uv).w;
	// TAG: areaLight no shadow
    float sum = len * scale;
    // float sum = len;
    if (!behind)
        sum = 0.0;

    // Outgoing radiance (solid angle) for the entire polygon
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

vec4 areaLightEffect(vec3 N, vec3 P, vec3 Kd, vec3 Ks)
{
	// gamma correction
    Ks = ToLinear(Ks);

    N = normalize(N);
    vec3 V = normalize(viewPosition - P);
    float dotNV = clamp(dot(N, V), 0.0f, 1.0f);

    // use roughness and sqrt(1-cos_theta) to sample M_texture
    vec2 uv = vec2(areaLight.roughness, sqrt(1.0f - dotNV)) * LUT_SCALE + LUT_BIAS;
    // get 4 parameters for inverse_M
    vec4 t1 = texture(texture10, uv);
    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(texture11, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    // translate light source for testing
    vec3 translatedPoints[4];
    translatedPoints[0] = (areaLight.rotateMat * vec4(areaLight.points[0], 1.0)).xyz + areaLight.translate;
    translatedPoints[1] = (areaLight.rotateMat * vec4(areaLight.points[1], 1.0)).xyz + areaLight.translate;
    translatedPoints[2] = (areaLight.rotateMat * vec4(areaLight.points[2], 1.0)).xyz + areaLight.translate;
    translatedPoints[3] = (areaLight.rotateMat * vec4(areaLight.points[3], 1.0)).xyz + areaLight.translate;

    // Evaluate LTC shading
    vec3 diffuse = LTC_Evaluate(N, V, P, mat3(1), translatedPoints);
    // vec3 specular = LTC_Evaluate(N, V, P, Minv, translatedPoints);

    // GGX BRDF shadowing and Fresnel
    // t2.x: shadowedF90 (F90 normally it should be 1.0)
    // t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
    // specular *= Ks*t2.x + (1.0f - Ks) * t2.y;

    vec3 result = areaLight.color * areaLight.intensity * (Kd * ToSRGB(diffuse));

    return vec4(result, 1.0f);
}

void defaultDraw()
{
	vec3 vertex = texture(texture1, texCoords).xyz;
	vec3 normal = texture(texture2, texCoords).xyz;
	vec3 tangent = texture(texture6, texCoords).xyz;
	vec3 SSAO = blurSSAO();

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
	vec3 originN = N;


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

	if (!blinnPhongEnabled && !bloomEffectEnabled && !areaLightEnabled)
	{
		// Default render: Kd and Kd_map
		color0 = vec4(texture(texture4, texCoords).xyz, 1.0);
		if (SSAOEnabled)
			color0 *= vec4(SSAO, 1.0);
	}
	else
	{
		color0 = vec4(0.0);
		if (blinnPhongEnabled)
		{
			// Blinn Phong render
			if (renderType == 5)
			{
				color0 = vec4(1.0, 1.0, 1.0, 100.0);
			}
			else
			{
				vec4 shadow_coord = shadow_sbpv * vec4(texture(texture1, texCoords).xyz,1);
				float shadow = textureProj(texture8, shadow_coord+ vec4(0,0,-0.02,0));
				if(directionalShadowEnabled)
					color0 += blinnNPR(N, L, H, shadow, SSAO);
				else
					color0 += blinnNPR(N, L, H, 1.0f, SSAO);
			}
		}
		if (bloomEffectEnabled)
		{
			// Blinn Phong render
			if (renderType == 0 || renderType == 4)
			{
				color0 += CalcPointLight(N, pointlight, P.xyz, SSAO);
			}
			else if (renderType == 1)
			{
				color0 += CalcPointLightNormalMap(N, pointlight, P.xyz, T, B, V, originN, SSAO);
			}
			else if (renderType == 2)
			{
				color0 = vec4(1.0, 1.0, 1.0, 100.0);
			}
		}
		if (areaLightEnabled)
		{
			if (renderType == 3)
			{
				color0 = vec4(areaLight.color, 1.0);
			}
			else
			{
				vec3 worldN = normalize(normal);
				vec3 Kd = texture(texture4, texCoords).xyz;
				vec3 Ks = texture(texture5, texCoords).xyz;
				if (renderType == 1)
				{
					vec3 NN = normalize(normal);
					vec3 NT = normalize(tangent);
					vec3 NB = cross(NN, NT);
					mat3 NTBN = mat3(NT, NB, NN);
					worldN = normalize(NTBN * texture(texture7, texCoords).xyz);
				}
				color0 += areaLightEffect(worldN, vertex, Kd, Ks);
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
		case 7:
			// vec2 noiseScale = vec2(frameSize.x / 40.0, frameSize.y / 40.0);
			// color0 = vec4(vec3(texture(texture6, texCoords).w), 1.0);
			// color0 = texture(texture0, texCoords);
			color0 = vec4(ShadowCalculation());
			// color0 = vec4(vec3(texture(texture6, texCoords).w), 1.0);

			break;
		default:
			color0 = vec4(1.0);
			break;
	}
}

void copyColor()
{
	color1 = texture(texture1, texCoords);
	color2 = texture(texture2, texCoords);
	color3 = texture(texture3, texCoords);
	color4 = texture(texture4, texCoords);
	color5 = texture(texture5, texCoords);
}

void genProjectionColor()
{
	vec4 color = texture(texture2, texCoords);
	float renderType = texture(texture2, texCoords).w;
	if (renderType == 5)
		color6 = vec4(vec3(1.0), 1.0);
	else if (color.xyz == vec3(0.2))
		color6 = vec4(vec3(0.19), 1.0);
	else
		color6 = vec4(vec3(0.0), 1.0);
}

void main()
{
	gBufferDraw();
	copyColor();
	genProjectionColor();
}