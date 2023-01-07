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

    void guiMenu(
        Camera &camera, 
        bool &blinnPhongEnabled, 
        bool &directionalShadowEnabled, 
        bool &pointShadowEnabled,
        bool &normalMappingEnabled, 
        bool &bloomEffectEnabled, 
        bool &effectTestMode, 
        bool& SSAOEnable, 
        bool &FXAAEnabled, 
        bool &areaLightEnabled,
        int &gBufferMode, 
        float *cameraPosition, 
        float *cameraLookAt, 
        vec3 &directionalShadowPosition,
        vec3 &pointShadowPosition,
        vec3 &areaLightPosition,
        vec3 &areaLightRotate);

    void menuCleanup();

    void setWidth(int val);
    void setHeight(int val);

private:
    int width;
    int height;

    void setupStyle();
};

#endif
