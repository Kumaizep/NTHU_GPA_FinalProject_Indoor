#include "imguiPanel.hpp"

ImguiPanel::ImguiPanel(){}
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
}

void ImguiPanel::guiMenu(Camera &camera, bool &blinnPhongEnabled, bool &directionalShadowEnabled,
    bool &normalMappingEnabled, bool &bloomEffectEnabled, bool &effectTestMode, 
    float *cameraPosition, float *cameraLookAt, int &gBufferMode)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(INIT_WIDTH + 2, 0));
    ImGui::SetNextWindowPos(ImVec2(-1 + 5, 0 + 5));
    ImGui::Begin("Menu", NULL
                // ImGuiWindowFlags_NoTitleBar |
                // ImGuiWindowFlags_NoBringToFrontOnFocus |
                // ImGuiWindowFlags_NoBackground |
                // ImGuiWindowFlags_NoMove |
                // ImGuiWindowFlags_NoResize
                // ImGuiWindowFlags_MenuBar
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
    ImGui::SameLine(100);
    ImGui::PushItemWidth(240);
    ImGui::InputFloat3("##Eye Position", cameraPosition);
    ImGui::PopItemWidth();
    ImGui::SameLine(350);
    ImGui::Text("Look-at Center:");
    ImGui::SameLine(450);
    ImGui::PushItemWidth(240);
    ImGui::InputFloat3("##look-at center", cameraLookAt);
    ImGui::PopItemWidth();
    ImGui::SameLine(700);
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