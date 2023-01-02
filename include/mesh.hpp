#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct Vertex
{
    // position
    vec3 position;
    // normal
    vec3 normal;
    // texCoords
    vec2 texCoords;
    // tangent
    vec3 tangent;
    // bitangent
    vec3 bitangent;
    // // bone indexes which will influence this vertex
    // int mBoneIDs[MAX_BONE_INFLUENCE];
    // // weights from each bone
    // float mWeights[MAX_BONE_INFLUENCE];
};

struct MaterialColor
{
    // ambient
    vec3 Ka;
    // diffuse
    vec3 Kd;
    // specular
    vec3 Ks;
    // emissive
    vec3 Ke;
    // refractive
    float Ni;
    // refractive
    float d;
    // shineness
    float Ns;
    //
    int illum;
};


class Mesh
{
public:
    vector<Vertex>  vertices;
    vector<GLuint>  indices;
    vector<Texture> textures;
    MaterialColor material;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, MaterialColor material);

    void draw(Shader& shader);

private:
    GLuint VAO, VBO, EBO;
    int haveMapDiffuse = 0;
    int haveMapHeight = 0;
    void setMesh();

    void drawSetup(Shader& shader);
};

#endif