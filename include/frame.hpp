#ifndef FRAME_HPP
#define FRAME_HPP

#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"

#define G_BUFFER_NUM 7

const GLfloat quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    }; 

class Frame
{
public:
    GLuint FBO;
    
    Frame();

    void draw(Shader& shader);

    void setFrameSize(int width, int height);

    void updateFrameBufferObject();

    GLuint FBT;
private:
    GLuint GBuffer[G_BUFFER_NUM];
    GLuint quadVAO;
    GLuint depthRenderBuffer;
    vector<Texture> filterTextures;

    int frameWidth = INIT_WIDTH;
    int frameHeight = INIT_HEIGHT;
    
    void createFrameTextureObject();

    void bindGbufferTexture(int index, GLint internalformat, GLenum format, GLenum type);

    void createGBufferTextureObject();

    void createDeepBuffer();


    void createFrameRenderObject();

    void createFrameVextexObject();
};

#endif