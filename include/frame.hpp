#ifndef FRAME_HPP
#define FRAME_HPP

#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"

#define G_BUFFER_NUM 6

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
    

    Frame()
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
        // glDeleteFramebuffers(1, &FBO);

        createFrameVextexObject();

        // filterTextures.push_back(Texture("asset/textures/noise_texture_0001.png", "textureUnknow"));
    }

    void draw(Shader& shader)
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
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void setFrameSize(int width, int height)
    {
        frameWidth = width;
        frameHeight = height;
    }

    void updateFrameBufferObject()
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

    GLuint FBT;
private:
    GLuint GBuffer[6];
    GLuint quadVAO;
    vector<Texture> filterTextures;

    int frameWidth = INIT_WIDTH;
    int frameHeight = INIT_HEIGHT;
    
    void createFrameTextureObject()
    {
        glGenTextures(1, &FBT);
        glBindTexture(GL_TEXTURE_2D, FBT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);
    }

    void createGBufferTextureObject()
    {
        glGenTextures(G_BUFFER_NUM, GBuffer);

        // world space vertex
        glBindTexture(GL_TEXTURE_2D, GBuffer[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GBuffer[0], 0);

        // world space normal
        glBindTexture(GL_TEXTURE_2D, GBuffer[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GBuffer[1], 0);

        // ambient color map
        glBindTexture(GL_TEXTURE_2D, GBuffer[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, frameWidth, frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, GBuffer[2], 0);

        // diffuse color map
        glBindTexture(GL_TEXTURE_2D, GBuffer[3]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, frameWidth, frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, GBuffer[3], 0);
        
        // specular color map
        glBindTexture(GL_TEXTURE_2D, GBuffer[4]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, frameWidth, frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, GBuffer[4], 0);

        glBindTexture(GL_TEXTURE_2D, GBuffer[5]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, GBuffer[5], 0);

        GLuint attachments[G_BUFFER_NUM + 1] = { 
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2, 
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5,
            GL_COLOR_ATTACHMENT6
        };
        glDrawBuffers(G_BUFFER_NUM + 1, attachments);
    }

    void createFrameRenderObject()
    {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, frameWidth, frameHeight);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    void createFrameVextexObject()
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
};

#endif