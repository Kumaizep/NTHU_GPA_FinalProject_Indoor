#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include "common.hpp"
#include "shader.hpp"

static const char* textureTypes[] = {
    "textureNone",
    "textureDiffuse", 
    "textureSpecular", 
    "textureAmbient", 
    "textureEmissive", 
    "textureHeight", 
    "textureNormal", 
    "textureShininess", 
    "textureOpacity", 
    "textureDisplacement", 
    "textureLightmap", 
    "textureReflection", 
    "textureUnknow"
};

static const aiTextureType aiTextureTypes[] = {
    aiTextureType_NONE, 
    aiTextureType_DIFFUSE, 
    aiTextureType_SPECULAR, 
    aiTextureType_AMBIENT, 
    aiTextureType_EMISSIVE, 
    aiTextureType_HEIGHT, 
    aiTextureType_NORMALS, 
    aiTextureType_SHININESS, 
    aiTextureType_OPACITY, 
    aiTextureType_DISPLACEMENT, 
    aiTextureType_LIGHTMAP, 
    aiTextureType_REFLECTION, 
    aiTextureType_UNKNOWN
};

class Texture
{
public:
    GLuint id;
    string type;
    string path;
    int width;
    int height;
    int colorChannel;

	Texture(const string &filepath, string typeName);

	GLuint loadTexture(const string &path, int &width, int &height);

    int comparePath(const char* filepath);

	void activeAndBind(Shader& shader, GLuint unit);
};

#endif
