#ifndef SHADOW_FRAME_HPP
#define SHADOW_FRAME_HPP

#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"

struct ShadowFrame
{
    // For Frame Buffer Object(FBO)
    GLuint FBO;
    GLuint depth_tex;
    bool isPointLight;

    ShadowFrame(bool ipl);

    void enable();

    void createFrameTextureObject();
};

#endif