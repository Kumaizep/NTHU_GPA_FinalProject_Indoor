#include "texture.hpp"

Texture::Texture()
{
    type = "textureUnknow";
    path = "##NULL";
}

Texture::Texture(const string &filepath, string typeName)
{
    id = loadTexture(filepath, width, height);
    type = typeName;
    path = filepath;
}

GLuint Texture::loadTexture(const string &path, int &width, int &height)
{  
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &colorChannel, 0);

    std::cout << "DEBUG::TEXTURE::LT::Texture info: w" << width << " h" << height << " ch" << colorChannel << std::endl;

    GLuint textureID;
    glGenTextures(1, &textureID);

    if (data)
    {
        GLint internalFormat;
        GLint format;
        switch (colorChannel) {
            case 1:
                internalFormat = GL_R8;
                format = GL_RED;
                break;
            case 2:
                internalFormat = GL_RG8;
                format = GL_RG;
                break;
            case 3:
                internalFormat = GL_RGB8;
                format = GL_RGB;
                break;
            case 4:
                internalFormat = GL_RGBA8;
                format = GL_RGBA;
                break;
            default:
                cout << "ERROR::Texture with unknow color channel " << colorChannel << ": " << path << endl;
                break;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR::Texture failed to load at path: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

int Texture::comparePath(const char* filepath)
{
    return strcmp(path.data(), filepath);
}

void Texture::activeAndBind(Shader& shader, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
    shader.setInt(("colorChannel" + to_string(unit)).c_str(), colorChannel);
    // cout << "DEBUG::TEXTURE::AAB::path: " << path << endl;
    // cout << "DEBUG::TEXTURE::AAB::colorChannel" + to_string(unit) << ": " << colorChannel << endl;
    // shader.setVec2("textureSizeReciprocal", 1.0f / width, 1.0f / height);
}

void Texture::generateRandomNoise()
{
    id = RandomNoise();
}

GLuint Texture::RandomNoise()
{
    vec3 noiseData[16];
    srand(time(NULL));
    for (int i = 0; i < 16; ++i)
    {
        noiseData[i] = normalize(vec3(
                rand() / (float)RAND_MAX * 2.0 - 1.0, // -1.0 ~ 1.0
                rand() / (float)RAND_MAX * 2.0 - 1.0, // -1.0 ~ 1.0
                0.0f
            ));
    }

    GLuint noiseMap;
    glGenTextures(1, &noiseMap);
    glBindTexture(GL_TEXTURE_2D, noiseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 4, 4, 0, GL_RGB, GL_FLOAT, &noiseData[0][0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return noiseMap;
}
