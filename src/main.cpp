#include "common.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "frame.hpp"
#include <vector>

mat4 view(1.0f);                    // V of MVP, viewing matrix
mat4 projection(1.0f);              // P of MVP, projection matrix

GLint um4p;
GLint um4mv;
GLint tex;

GLubyte timerCounter = 0;
bool timerEnabled = true;
float timerCurrent = 0.0f;
float timerLast = 0.0f;
unsigned int timerSpeed = 16;

bool keyPressing[400] = {0};
float keyPressTime[400] = {0.0f};

bool trackballEnable = false;
vec2 mouseCurrent = vec2(0.0f, 0.0f);
vec2 mouseLast = vec2(0.0f, 0.0f);

int guiMenuWidth = INIT_WIDTH;
int frameWidth = INIT_WIDTH;
int frameHeight = INIT_HEIGHT;

int outputMode = 0;
int filterMode = 0;
int testMode = 0;

float cameraPosition[] = {1.0, 1.0, 1.0};
float cameraLookAt[] = {1.0, 1.0, 1.0};

bool normalMappingEnabled = false;

bool needUpdateFBO = false;

vector<Model> models;

const char* filterTypes[] = {
    "Default",
    "Image Abstraction",
    "Watercolor",
    "Magnifier", 
    "Bloom Effect", 
    "Pixelization", 
    "Sine Wave"
};

void initialization(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
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

    timerLast = glfwGetTime();
    mouseLast = vec2(0.0f, 0.0f);   
}

void timerUpdate()
{
    timerLast = timerCurrent;
    timerCurrent = glfwGetTime();
}

void setGUICameraStatus(Camera& camera)
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

void processCameraMoveWithDirection(Camera& camera, MoveDirection moveDirction, float timeDifferent)
{
    camera.processMove(moveDirction, timeDifferent);
    setGUICameraStatus(camera);
}

void processCameraMove(Camera& camera)
{
    float timeDifferent = 0.0f;
    if (timerEnabled)
        timeDifferent = timerCurrent - timerLast;

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

void processCameraTrackball(Camera& camera, GLFWwindow *window)
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

void updateFrameVariable(Frame& frame)
{
    frame.setTestMode(testMode);
    frame.setFilterMode(filterMode);
    frame.setFrameSize(frameWidth, frameHeight);
    if (needUpdateFBO)
    {
        frame.updateFrameBufferObject();
        needUpdateFBO = false;
    }
}

void display(Shader& shader, Camera& camera)
{
    if (timerEnabled) timerCounter += 1.0f;

    shader.use();

    projection = camera.getPerspective();
    view = camera.getView();
    shader.setMat4("um4p", projection);
    shader.setMat4("um4v", view);
    shader.setMat4("um4m", mat4(1.0f));
    shader.setInt("outputMode", outputMode);
    shader.setBool("normalMappingEnabled", normalMappingEnabled);

    for (auto& it : models)
    {
        shader.setMat4("um4v", view);
        shader.setMat4("um4m", it.getModelMatrix());
        it.draw(shader);
    }
}

void windowUpdate(Shader& frameShader, Shader& shader, Camera& camera, Frame& frame)
{
    updateFrameVariable(frame);

    // Update to Frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frame.FBO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
    glEnable(GL_DEPTH_TEST);
    display(shader, camera);

    // Update to window
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    frame.draw(frameShader);
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
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_T:
            if (action == GLFW_PRESS) timerEnabled = !timerEnabled;
            break;
        case GLFW_KEY_G:
            if (action == GLFW_PRESS) testMode = (testMode + 1) % 2;
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
        if (action == GLFW_PRESS) {
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {  
        if (action == GLFW_PRESS) {
            trackballEnable = true;
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
            trackballEnable = false;
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
}



void guiMenu(Camera& camera)
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

    ImGui::Checkbox("Normal Mapping", &normalMappingEnabled);

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
    GLFWwindow* window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Final Project Indoor", NULL, NULL);
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
    Shader shader("assets/vertex.vs.glsl", "assets/fragment.fs.glsl");
    Shader frameShader("assets/frameVertex.vs.glsl", "assets/frameFragment.fs.glsl");
    Camera camera = Camera()
                        .withPosition(vec3(4.0f, 1.0f, -1.5f))
                        .withFar(5000.0f)
                        .withMoveSpeed(3.0f)
                        .withTheta(180.0f);
    setGUICameraStatus(camera);

    cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << endl;
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
        windowUpdate(frameShader, shader, camera, frame);
        guiMenu(camera);

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }

    menuCleanup();
    // just for compatibiliy purposes
    return 0;
}
