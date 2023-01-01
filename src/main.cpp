#include "common.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "frame.hpp"
#include "shadowFrame.hpp"
#include <vector>

float timerCurrent = 0.0f;
float timerLast = 0.0f;
unsigned int timerSpeed = 16;

// Keyboard control
bool keyPressing[400] = {0};
float keyPressTime[400] = {0.0f};

// Mouse control
bool trackballEnable = false;
vec2 mouseCurrent = vec2(0.0f, 0.0f);
vec2 mouseLast = vec2(0.0f, 0.0f);

int guiMenuWidth = INIT_WIDTH;
int frameWidth = INIT_WIDTH;
int frameHeight = INIT_HEIGHT;

float cameraPosition[] = {1.0, 1.0, 1.0};
float cameraLookAt[] = {1.0, 1.0, 1.0};

int gBufferMode = 0;
bool effectTestMode = false;
bool effectTestMode2 = false;

bool blinnPhongEnabled = false;
bool normalMappingEnabled = false;
bool bloomEffectEnabled = false;

bool needUpdateFBO = false;

vector<Model> models;
vector<Model> lights;

void initialization(GLFWwindow *window)
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
    ImGui_ImplOpenGL3_Init("#version 410 core");

    models.push_back(Model("assets/indoor/Grey_White_Room.obj"));
    models.push_back(Model("assets/indoor/trice.obj")
                         .withPosition(vec3(2.05, 0.628725, -1.9))
                         .withScale(vec3(0.001, 0.001, 0.001)));

    lights.push_back(Model("assets/indoor/Sphere.obj")
                         .withPosition(vec3(1.87659, 0.4625, 0.103928))
                         .withScale(vec3(0.22, 0.22, 0.22)));

    timerLast = glfwGetTime();
    mouseLast = vec2(0.0f, 0.0f);
}

void timerUpdate()
{
    timerLast = timerCurrent;
    timerCurrent = glfwGetTime();
}

void setGUICameraStatus(Camera &camera)
{
    vec3 positionVector = camera.getPosition();
    vec3 lookAtVector = camera.getLookAt();
    cameraPosition[0] = positionVector.x;
    cameraPosition[1] = positionVector.y;
    cameraPosition[2] = positionVector.z;
    cameraLookAt[0] = lookAtVector.x;
    cameraLookAt[1] = lookAtVector.y;
    cameraLookAt[2] = lookAtVector.z;
}

void processCameraMoveWithDirection(Camera &camera, MoveDirection moveDirction, float timeDifferent)
{
    camera.processMove(moveDirction, timeDifferent);
    setGUICameraStatus(camera);
}

void processCameraMove(Camera &camera)
{
    float timeDifferent = timerCurrent - timerLast;

    if (keyPressing[GLFW_KEY_W])
        processCameraMoveWithDirection(camera, FORWARD, timeDifferent);
    if (keyPressing[GLFW_KEY_S])
        processCameraMoveWithDirection(camera, BACKWARD, timeDifferent);
    if (keyPressing[GLFW_KEY_A])
        processCameraMoveWithDirection(camera, LEFT, timeDifferent);
    if (keyPressing[GLFW_KEY_D])
        processCameraMoveWithDirection(camera, RIGHT, timeDifferent);
    if (keyPressing[GLFW_KEY_Z])
        processCameraMoveWithDirection(camera, UP, timeDifferent);
    if (keyPressing[GLFW_KEY_X])
        processCameraMoveWithDirection(camera, DOWN, timeDifferent);
}

void processCameraTrackball(Camera &camera, GLFWwindow *window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouseLast = mouseCurrent;
    mouseCurrent = vec2(x, y);

    if (trackballEnable)
    {
        vec2 mouseDifferent = vec2(0.0f, 0.0f);
        mouseDifferent = mouseCurrent - mouseLast;
        camera.processTrackball(mouseDifferent.x, mouseDifferent.y);
        setGUICameraStatus(camera);
    }
}

void setupDeferredShaderUniform(Shader &shader, Camera &camera)
{
    shader.use();

    shader.setMat4("um4v", camera.getView());

    vec4 lightpos = camera.getView() * vec4(1.87659f, 0.4625f, 0.103928f, 1);
    shader.setVec3("pointlight.position", vec3(lightpos));
    shader.setFloat("pointlight.constant", 1.0f);
    shader.setFloat("pointlight.linear", 0.7f);
    shader.setFloat("pointlight.quadratic", 0.14f);

    shader.setInt("gBufferMode", gBufferMode);
    shader.setBool("effectTestMode", effectTestMode);
    shader.setBool("blinnPhongEnabled", blinnPhongEnabled);
    shader.setBool("normalMappingEnabled", normalMappingEnabled);
    shader.setBool("bloomEffectEnabled", bloomEffectEnabled);

    shader.setVec2("frameSize", (float)frameWidth, (float)frameHeight);
    // cout << "DEBUG::MAIN::SFSU:effectTestMode " << (effectTestMode? 1.0 : 0.5) << endl;
}

void setupFrameShaderUniform(Shader &shader, Camera &camera)
{
    shader.use();

    shader.setInt("gBufferMode", gBufferMode);
    shader.setBool("bloomEffectEnabled", bloomEffectEnabled);
    shader.setBool("effectTestMode", effectTestMode);
    shader.setVec2("frameSize", (float)frameWidth, (float)frameHeight);
    // cout << "DEBUG::MAIN::SFSU:effectTestMode " << (effectTestMode? 1.0 : 0.5) << endl;
}

void updateFrameVariable(Frame &frame)
{
    frame.setFrameSize(frameWidth, frameHeight);
    frame.updateFrameBufferObject();
}

void display(Shader &shader, Camera &camera, bool ShadowMode)
{
    shader.use();

    if (ShadowMode)
        shader.setMat4("um4p", ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f));
    else
        shader.setMat4("um4p", camera.getPerspective());
    shader.setMat4("um4v", camera.getView());
    shader.setMat4("um4m", mat4(1.0f));
    shader.setBool("blinnPhongEnabled", blinnPhongEnabled);
    shader.setBool("normalMappingEnabled", normalMappingEnabled);
    shader.setBool("bloomEffectEnabled", bloomEffectEnabled);
    shader.setInt("gBufferMode", gBufferMode);

    vec4 lightpos = camera.getView() * vec4(1.87659f, 0.4625f, 0.103928f, 1);

    shader.setVec3("pointlight.position", vec3(lightpos));
    shader.setFloat("pointlight.constant", 1.0f);
    shader.setFloat("pointlight.linear", 0.7f);
    shader.setFloat("pointlight.quadratic", 0.14f);

    shader.setBool("lightMode", 0);
    for (auto &it : models)
    {
        shader.setMat4("um4m", it.getModelMatrix());
        it.draw(shader);
    }
    if (bloomEffectEnabled)
    {
        shader.setBool("lightMode", 1);
        for (auto &it : lights)
        {
            shader.setMat4("um4m", it.getModelMatrix());
            it.draw(shader);
        }
    }
}

void windowUpdate(Shader &frameShader, Shader &deferredShader, Shader &shadowShader, Shader &shader, Camera &camera, Camera &shadowCamera, Frame &deferredFrame, Frame &frame, ShadowFrame &shadowFrame)
{
    if (needUpdateFBO)
    {
        updateFrameVariable(deferredFrame);
        updateFrameVariable(frame);
        needUpdateFBO = false;
    }

    // render from light view and generate shadow map
    shadowFrame.enable();
    display(shadowShader, shadowCamera, 1);

    // setup depthmap at GL_TEXTURE3
    deferredShader.use();
    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, shadowFrame.depth_tex);
    // make shadow vp matrix
    mat4 scale_bias(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0);
    deferredShader.setMat4("shadow_sbpv", scale_bias * ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f) * shadowCamera.getView());

    // Draw scene to frame.FBO
    glViewport(0, 0, frameWidth, frameHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, frame.FBO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
    glEnable(GL_DEPTH_TEST);
    display(shader, camera, 0);

    // Draw frame with deferredShader to frame.FBO
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFrame.FBO); // back to default
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    setupDeferredShaderUniform(deferredShader, camera);
    frame.draw(deferredShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    setupFrameShaderUniform(frameShader, camera);
    deferredFrame.draw(frameShader);
}

void reshapeResponse(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    frameWidth = guiMenuWidth = width;
    frameHeight = height;
    needUpdateFBO = true;
}

void keyboardResponse(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_A:
    case GLFW_KEY_W:
    case GLFW_KEY_S:
    case GLFW_KEY_Z:
    case GLFW_KEY_X:
        if (action == GLFW_PRESS)
        {
            keyPressing[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keyPressing[key] = false;
        }
        break;
    default:
        break;
    }
}

void mouseResponse(GLFWwindow *window, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE)
        {
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            trackballEnable = true;
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE)
        {
            trackballEnable = false;
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
}

void guiMenu(Camera &camera)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(guiMenuWidth + 2, 0));
    ImGui::SetNextWindowPos(ImVec2(-1, 0));
    ImGui::Begin("Menu", NULL,
                 ImGuiWindowFlags_NoTitleBar |
                     // ImGuiWindowFlags_NoBringToFrontOnFocus |
                     // ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize
                 // ImGuiWindowFlags_MenuBar
    );

    ImGui::Checkbox("Blinn-Phong", &blinnPhongEnabled);
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

void menuCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int main(int argc, char **argv)
{
    // initial glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // specifies whether to use full resolution framebuffers on Retina displays
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    // create window
    GLFWwindow *window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Final Project Indoor", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // load OpenGL function pointer
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    dumpInfo();
    Shader shader(
        "assets/shader/baseVertex.vs.glsl", "assets/shader/baseFragment.fs.glsl");
    Shader shadowShader(
        "assets/shader/shadowVertex.vs.glsl", "assets/shader/shadowFragment.fs.glsl");
    Shader deferredShader(
        "assets/shader/deferredVertex.vs.glsl", "assets/shader/deferredFragment.fs.glsl");
    Shader frameShader(
        "assets/shader/frameVertex.vs.glsl", "assets/shader/frameFragment.fs.glsl");
    Camera camera = Camera()
                        .withPosition(vec3(4.0f, 1.0f, -1.5f))
                        .withFar(5000.0f)
                        .withMoveSpeed(3.0f)
                        .withTheta(180.0f);
    Camera shadowCamera = Camera()
                              .withPosition(vec3(-2.845f, 2.028f, -1.293f))
                              .withMoveSpeed(3.0f)
                              .withTheta(180.0f);
    shadowCamera.setLookAt(vec3(0.542f, -0.141f, -0.422f));
    setGUICameraStatus(camera);

    // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << endl;
    ShadowFrame shadow;
    Frame deferredFrame = Frame();
    Frame frame = Frame();
    initialization(window);

    // register glfw callback functions
    glfwSetFramebufferSizeCallback(window, reshapeResponse);
    glfwSetKeyCallback(window, keyboardResponse);
    glfwSetMouseButtonCallback(window, mouseResponse);

    cout << "DEBUG::MAIN::F-MAIN::1" << endl;
    // main loop
    float timeDifferent = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // Poll input event
        // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << endl;

        glfwPollEvents();
        timerUpdate();

        processCameraMove(camera);
        processCameraTrackball(camera, window);
        windowUpdate(frameShader, deferredShader, shadowShader, shader, camera, shadowCamera, deferredFrame, frame, shadow);
        guiMenu(camera);

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }

    menuCleanup();
    // just for compatibiliy purposes
    return 0;
}
