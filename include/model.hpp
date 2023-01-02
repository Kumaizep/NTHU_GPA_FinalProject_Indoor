#ifndef MODEL_HPP
#define MODEL_HPP

#include "GLM/fwd.hpp"
#include "mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "texture.hpp"

struct ImageData
{
    int width;
    int height;
    GLenum format;
    unsigned char* data;
};


class Model 
{
public:
    Model(const string path);

    Model& withPosition(vec3 val);

    Model& withScale(vec3 val);

    void draw(Shader& shader);

    mat4 getModelMatrix();

    vec3 position = vec3(0.0f);
private:
    vector<Mesh> meshes;
    string directory;
    vec3 scale3D = vec3(1.0f);
    mat4 modelMatrix = mat4(1.0f);

    void loadModel(const string path);

    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    vector<Vertex> processVertices(aiMesh* mesh);

    vector<GLuint> processIndices(aiMesh* mesh);

    MaterialColor processMaterialColor(aiMesh* mesh, const aiScene* scene);

    vector<Texture> processTextures(aiMesh* mesh, const aiScene* scene);


    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

    int getLoadedTextureId(string str);
};

#endif