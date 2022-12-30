#version 450

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 iv3normal;
layout(location = 2) in vec2 iv2tex_coord;
layout(location = 3) in vec3 iv3tangent;
layout(location = 4) in vec3 iv3bitangent;

uniform mat4 um4v;
uniform mat4 um4m;
uniform mat4 um4p;

out VertexData
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

void main()
{
    vec4 P = um4v * um4m * vec4(iv3vertex, 1.0);
    vec4 LW = um4v * vec4(-2.845, 2.028, -1.293, 1.0);
    // Eye space to tangent space TBN
    vec3 T = normalize(mat3(um4v * um4m) * iv3tangent);
    vec3 N = normalize(mat3(um4v * um4m) * iv3normal);
    vec3 B = cross(N, T);
    vec3 L = LW.xyz - P.xyz;
    vec3 V = -P.xyz;

    vec3 TBNL = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
    vec3 TBNV = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));
    vertexData.TBNL = TBNL;
    vertexData.TBNV = TBNV;
    vertexData.TBNH = normalize(TBNL + TBNV);

    vertexData.L = normalize(L);
    vertexData.H = normalize(vertexData.L + normalize(V));
    vertexData.N = N;
    vertexData.P = P.xyz;
    vertexData.T = T;
    vertexData.B = B;
    
    vertexData.worldPosition = (um4m * vec4(iv3vertex, 1.0)).xyz;
    vertexData.normal = iv3normal;
    vertexData.texcoord = iv2tex_coord;
    vertexData.tangent = iv3tangent;
    vertexData.bitangent = iv3bitangent;

    gl_Position = um4p * P;
}
