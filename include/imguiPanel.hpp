#ifndef IMGUIPANEL_HPP
#define IMGUIPANEL_HPP

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "common.hpp"
#include "camera.hpp"

class ImguiPanel
{
public:
    ImguiPanel(int width, int height = 0);
    ~ImguiPanel();

    void initinalize(GLFWwindow *window);

    void guiMenu(Camera &camera, bool &blinnPhongEnabled, bool &directionalShadowEnabled,
        bool &normalMappingEnabled, bool &bloomEffectEnabled, bool &effectTestMode, 
        float *cameraPosition, float *cameraLookAt, int &gBufferMode, vec3 &directionalShadowPosition);

    void menuCleanup();

    void setWidth(int val);
    void setHeight(int val);
private:
    int width;
    int height;

    void setupStyle();
};

#endif
