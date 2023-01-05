#include "frame.hpp"

Frame::Frame()
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    createFrameTextureObject();
    createGBufferTextureObject();
    createDeepBuffer();

    createFrameRenderObject();

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAME::INIT: Framebuffer is not complete!" << endl;
    else
        cout << "DEBUG::FRAME::INIT: Framebuffer is complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDeleteFramebuffers(1, &FBO);

    createFrameVextexObject();

    // filterTextures.push_back(Texture("asset/textures/noise_texture_0001.png", "textureUnknow"));
}

void Frame::draw(Shader& shader)
{
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, FBT);
    for (int i = 0; i < G_BUFFER_NUM; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, GBuffer[i]);
    }
    // int unit = 1;
    // for (auto& it: filterTextures) {
    //     it.activeAndBind(shader, unit++);
    // }
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, depthRenderBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Frame::setFrameSize(int width, int height)
{
    frameWidth = width;
    frameHeight = height;
}

void Frame::updateFrameBufferObject()
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    createFrameTextureObject();
    createGBufferTextureObject();

    createFrameRenderObject();

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAME::INIT: Framebuffer is not complete!" << endl;
    else
        cout << "DEBUG::FRAME::INIT: Framebuffer is complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Frame::createFrameTextureObject()
{
    glGenTextures(1, &FBT);
    glBindTexture(GL_TEXTURE_2D, FBT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);
}

void Frame::bindGbufferTexture(int index, GLint internalformat, GLenum format, GLenum type)
{
    glBindTexture(GL_TEXTURE_2D, GBuffer[index]);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, frameWidth, frameHeight, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + index, GL_TEXTURE_2D, GBuffer[index], 0);
}

void Frame::createGBufferTextureObject()
{
    glGenTextures(G_BUFFER_NUM, GBuffer);

    // world space vertex
    bindGbufferTexture(0, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    // world space normal
    bindGbufferTexture(1, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    // ambient color map
    bindGbufferTexture(2, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

    // diffuse color map
    bindGbufferTexture(3, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    
    // specular color map
    bindGbufferTexture(4, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

    // world space tangent
    bindGbufferTexture(5, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    // normal mapping texture
    bindGbufferTexture(6, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    GLuint attachments[G_BUFFER_NUM + 1] = { 
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7
    };
    glDrawBuffers(G_BUFFER_NUM + 1, attachments);
}

void Frame::createDeepBuffer()
{
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frameWidth, frameHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
}

void Frame::createFrameRenderObject()
{
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, frameWidth, frameHeight);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void Frame::createFrameVextexObject()
{
    GLuint quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
}