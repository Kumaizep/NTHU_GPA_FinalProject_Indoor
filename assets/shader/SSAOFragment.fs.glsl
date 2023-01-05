#version 450

in vec2 texCoords;
layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;
layout(location = 4) out vec4 color4;
layout(location = 5) out vec4 color5;
layout(location = 6) out vec4 color6;
layout(location = 7) out vec4 color7;

layout(binding = 0) uniform sampler2D texture0; // Default
layout(binding = 1) uniform sampler2D texture1; // world space vertex, shininess
layout(binding = 2) uniform sampler2D texture2; // world space normal, renderType
layout(binding = 3) uniform sampler2D texture3; // ambient color map
layout(binding = 4) uniform sampler2D texture4; // diffuse color map
layout(binding = 5) uniform sampler2D texture5; // specular color map
layout(binding = 6) uniform sampler2D texture6; // world space tangent, depth
layout(binding = 7) uniform sampler2D texture7; // normal mapping texture
layout(binding = 8) uniform sampler2DShadow texture8; // shadow map
layout(binding = 9) uniform sampler2D texture9; // random noise
layout(binding = 10) uniform sampler2D texture10; // random noise

layout(std140) uniform SSAOKernals
{
	vec3 val[32];
};

uniform vec2 frameSize;

uniform bool SSAOEnabled;

uniform mat4 um4v;
uniform mat4 um4p;

uniform vec3 sampleKernel[64];

vec3 SSAO(vec3 position, vec3 normal)
{
	float depth = texture(texture6, texCoords).w;
	if(depth == 1.0) { return vec3(1.0); }
	vec2 noiseScale = vec2(frameSize.x / 4.0, frameSize.y / 4.0);
	vec3 randomvec = normalize(texture(texture9, texCoords * noiseScale).xyz);
	// compute local coordinate system (TBN)
	vec3 tangent = normalize(randomvec - normal * dot(randomvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal); // tangent to world

	const float radius = 0.5;
	float occlusion = 0.0;
	for(int i = 0; i < 64; ++i)
	{
		vec3 samplePos = position + tbn * sampleKernel[i] * radius;

		vec4 offset = vec4(samplePos, 1.0);
		offset = um4p * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = (um4v * vec4(texture(texture1, offset.xy).xyz, 1.0)).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + 0.025 ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1 - (occlusion / 64);
	return vec3(occlusion);
}

void copyColor()
{
	color1 = texture(texture1, texCoords);
	color2 = texture(texture2, texCoords);
	color3 = texture(texture3, texCoords);
	color4 = texture(texture4, texCoords);
	color5 = texture(texture5, texCoords);
	color6 = texture(texture6, texCoords);
	color7 = texture(texture7, texCoords);
}


void main()
{
	vec3 vertex = texture(texture1, texCoords).xyz;
	vec3 normal = texture(texture2, texCoords).xyz;
	vec4 P = um4v * vec4(vertex, 1.0);
	vec3 N = normalize(mat3(um4v) * normal);

	if (SSAOEnabled)
		color0 = vec4(SSAO(P.xyz, N), 1.0);
	else
		color0 = vec4(1.0);

	copyColor();
}