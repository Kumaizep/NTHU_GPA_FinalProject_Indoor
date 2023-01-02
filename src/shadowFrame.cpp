#include "shadowFrame.hpp"

ShadowFrame::ShadowFrame()
{
    glGenFramebuffers(1, &FBO); // Create FBO
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    createFrameTextureObject();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "DEBUG::SHADOWFRAME::INIT: Framebuffer not complete: " << fboStatus << std::endl;
    else
        cout << "DEBUG::SHADOWFRAME::INIT: Framebuffer is complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowFrame::enable()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1024, 1024);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 4.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShadowFrame::createFrameTextureObject()
{
    glGenTextures(1, &depth_tex); // Create fboDataTexture
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
}