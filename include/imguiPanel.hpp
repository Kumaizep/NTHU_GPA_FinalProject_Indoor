#ifndef IMGUIPANEL_HPP
#define IMGUIPANEL_HPP

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "common.hpp"
#include "camera.hpp"

class ImguiPanel
{
public:
    ImguiPanel();
    ~ImguiPanel();

    void initinalize(GLFWwindow *window);

    void guiMenu(Camera &camera, bool &blinnPhongEnabled, bool &directionalShadowEnabled,
        bool &normalMappingEnabled, bool &bloomEffectEnabled, bool &effectTestMode, 
        float *cameraPosition, float *cameraLookAt, int &gBufferMode);

    void menuCleanup();
};

#endif
