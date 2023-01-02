#include "imguiPanel.hpp"

ImguiPanel::ImguiPanel(int width, int height) : width(width), height(height){}
ImguiPanel::~ImguiPanel(){}

void ImguiPanel::initinalize(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSansCJK-Medium.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");

    setupStyle();
}

void ImguiPanel::guiMenu(Camera &camera, bool &blinnPhongEnabled, bool &directionalShadowEnabled,
    bool &normalMappingEnabled, bool &bloomEffectEnabled, bool &effectTestMode, 
    float *cameraPosition, float *cameraLookAt, int &gBufferMode)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(width - 10, 0));
    ImGui::SetNextWindowPos(ImVec2(-1 + 5, 0 + 5));
    ImGui::Begin("Menu", NULL, ImGuiWindowFlags_None 
                // | ImGuiWindowFlags_NoTitleBar 
                // | ImGuiWindowFlags_NoBringToFrontOnFocus 
                // | ImGuiWindowFlags_NoBackground 
                | ImGuiWindowFlags_NoMove
                // | ImGuiWindowFlags_NoResize 
                // | ImGuiWindowFlags_MenuBar 
    );

    ImGui::Checkbox("Blinn-Phong", &blinnPhongEnabled);
    if (!blinnPhongEnabled)
        directionalShadowEnabled = false;
    ImGui::SameLine();
    ImGui::Checkbox("Directional Shadow", &directionalShadowEnabled);
    if (directionalShadowEnabled)
        blinnPhongEnabled = true;
    ImGui::SameLine();
    ImGui::Checkbox("Normal Mapping", &normalMappingEnabled);
    ImGui::SameLine();
    ImGui::Checkbox("Bloom effect", &bloomEffectEnabled);
    ImGui::SameLine();
    ImGui::Checkbox("Test Mode", &effectTestMode);
    // ImGui::SameLine();
    // ImGui::Checkbox("Test Mode2", &effectTestMode2);

    ImGui::Text("Eye Position:");
    ImGui::SameLine();
    ImGui::PushItemWidth(240);
    ImGui::InputFloat3("##Eye Position", cameraPosition);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Text("Look-at Center:");
    ImGui::SameLine();
    ImGui::PushItemWidth(240);
    ImGui::InputFloat3("##look-at center", cameraLookAt);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Enter"))
    {
        cout << "DEBUG::MAIN::GUIM::sent Pos" << endl;
        camera.setPosition(vec3(cameraPosition[0], cameraPosition[1], cameraPosition[2]));
        camera.setLookAt(vec3(cameraLookAt[0], cameraLookAt[1], cameraLookAt[2]));
    }

    ImGui::Text("G-bufferMode:");
    ImGui::SameLine();
    ImGui::RadioButton("Default", &gBufferMode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Vertex", &gBufferMode, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Normal", &gBufferMode, 2);
    ImGui::SameLine();
    ImGui::RadioButton("Ambient", &gBufferMode, 3);
    ImGui::SameLine();
    ImGui::RadioButton("Diffuse", &gBufferMode, 4);
    ImGui::SameLine();
    ImGui::RadioButton("Specular", &gBufferMode, 5);
    ImGui::SameLine();
    ImGui::RadioButton("Disable", &gBufferMode, 6);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiPanel::menuCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImguiPanel::setWidth(int val)
{
    width = val;
}

void ImguiPanel::setHeight(int val)
{
    height = val;
}

void ImguiPanel::setupStyle()
{
    ImGuiStyle * style = &ImGui::GetStyle();
 
    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;
 
    // style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    // style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
    // style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    // style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 0.88f);
    // style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.7f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.7f);
    // style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    // style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    // style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    // style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    // style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    // style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    // style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    // style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    // style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    // style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    // style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    // style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    // style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    // style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    // style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    // style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    // style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    // style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    // style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    // style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    // style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    // style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    // style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    // style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    // style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    // style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    // style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    // style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    // style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    // style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    // style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    // style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    // style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}