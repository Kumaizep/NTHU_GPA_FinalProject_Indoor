#include "GLM/fwd.hpp"
#include "common.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "frame.hpp"
#include "shadowFrame.hpp"
#include "imguiPanel.hpp"

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

int frameWidth = INIT_WIDTH;
int frameHeight = INIT_HEIGHT;

float cameraPosition[] = {1.0, 1.0, 1.0};
float cameraLookAt[] = {1.0, 1.0, 1.0};
vec3 directionalShadowPosition = vec3(-2.845, 2.028, -1.293);
vec3 pointShadowPosition = vec3(1.87659, 0.4625 , 0.103928);

int gBufferMode = 0;
bool effectTestMode = false;
bool effectTestMode2 = false;

bool blinnPhongEnabled = false;
bool directionalShadowEnabled = false;
bool pointShadowEnabled = false;
bool normalMappingEnabled = false;
bool bloomEffectEnabled = false;
bool SSAOEnabled = false;
bool NPREnabled = false;
bool FXAAEnabled = false;

bool needUpdateFBO = false;

vector<Model> models;
vector<Model> lights;

vector<vec3> ssaoKernel;

unsigned int noiseTextureTemp;

Shader baseShader;
Shader shadowShader;
Shader pointShadowShader;
Shader SSAOShader;
Shader deferredShader;
Shader frameShader;
Shader FXAAShader;
Shader FXAAShader2;

float calcLerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void kernelGeneration()
{
    uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0, 
            randomFloats(generator) * 2.0 - 1.0, 
            randomFloats(generator) * 0.5 + 0.5
            );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = calcLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
}

void tempNoiseGen()
{
    uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    default_random_engine generator;
    vector<vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTextureTemp);
    glBindTexture(GL_TEXTURE_2D, noiseTextureTemp);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void initialization(GLFWwindow *window)
{
    kernelGeneration();
    tempNoiseGen();

    models.push_back(Model("assets/indoor/Grey_White_Room.obj"));
    models.push_back(Model("assets/indoor/trice.obj")
                         .withPosition(vec3(2.05, 0.628725, -1.9))
                         .withScale(vec3(0.001, 0.001, 0.001)));

    lights.push_back(Model("assets/indoor/Sphere.obj")
                         .withPosition(pointShadowPosition)
                         .withScale(vec3(0.22, 0.22, 0.22)));

    timerLast = glfwGetTime();
    mouseLast = vec2(0.0f, 0.0f);

    baseShader = Shader(
        "assets/shader/baseVertex.vs.glsl", "assets/shader/baseFragment.fs.glsl");
    shadowShader = Shader(
        "assets/shader/shadowVertex.vs.glsl", "assets/shader/shadowFragment.fs.glsl");
    pointShadowShader = Shader(
        "assets/shader/pointShadowVertex.vs.glsl", "assets/shader/pointShadowFragment.fs.glsl", "assets/shader/pointShadowGeometry.gs.glsl");
    SSAOShader = Shader(
        "assets/shader/SSAOVertex.vs.glsl", "assets/shader/SSAOFragment.fs.glsl");
    deferredShader = Shader(
        "assets/shader/deferredVertex.vs.glsl", "assets/shader/deferredFragment.fs.glsl");
    frameShader = Shader(
        "assets/shader/frameVertex.vs.glsl", "assets/shader/frameFragment.fs.glsl");
    FXAAShader = Shader(
        "assets/shader/FXAAVertex.vs.glsl", "assets/shader/FXAAFragment.fs.glsl");
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

void updateFrameVariable(Frame &frame)
{
    frame.setFrameSize(frameWidth, frameHeight);
    frame.updateFrameBufferObject();
}

enum s_mode
{
    off,
    directional,
    point
};

void setupShadowUniform(Shader &shader, vec3 lightPos)
{
    float near_plane = 0.22f;
    float far_plane = 10.0f;
    mat4 shadowProj = perspective(radians(90.0f), 1.0f, near_plane, far_plane);
    std::vector<mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * lookAt(lightPos, lightPos + vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * lookAt(lightPos, lightPos + vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * lookAt(lightPos, lightPos + vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * lookAt(lightPos, lightPos + vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * lookAt(lightPos, lightPos + vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * lookAt(lightPos, lightPos + vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f)));

    for (unsigned int i = 0; i < 6; ++i)
        shader.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
    shader.setFloat("far_plane", far_plane);
    shader.setVec3("lightPos", lightPos);
}

void setupShaderUniform(Shader &shader, Camera &camera, s_mode shadowMode = off)
{
    shader.use();

    // MVP Matrix
    if (shadowMode == point)
        setupShadowUniform(shader, lights[0].getPosition());
    else if (shadowMode == directional)
        shader.setMat4("um4p", ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f));
    else
        shader.setMat4("um4p", camera.getPerspective());
    shader.setMat4("um4v", camera.getView());
    shader.setMat4("um4m", mat4(1.0f));

    // point light source
    vec4 lightpos = camera.getView() * vec4(lights[0].getPosition(), 1);
    shader.setVec3("pointlight.worldPosition", lights[0].getPosition());
    shader.setVec3("pointlight.position", vec3(lightpos));
    shader.setFloat("pointlight.constant", 1.0f);
    shader.setFloat("pointlight.linear", 0.7f);
    shader.setFloat("pointlight.quadratic", 0.14f);

    // rendering flag
    shader.setInt("gBufferMode", gBufferMode);
    shader.setBool("blinnPhongEnabled", blinnPhongEnabled);
    shader.setBool("directionalShadowEnabled", directionalShadowEnabled);
    shader.setBool("pointShadowEnabled", pointShadowEnabled);
    shader.setBool("normalMappingEnabled", normalMappingEnabled);
    shader.setBool("bloomEffectEnabled", bloomEffectEnabled);
    shader.setBool("SSAOEnabled", SSAOEnabled);
    shader.setBool("NPREnabled", NPREnabled);
    shader.setBool("FXAAEnabled", FXAAEnabled);
    shader.setBool("effectTestMode", effectTestMode);

    // others
    shader.setVec2("frameSize", (float)frameWidth, (float)frameHeight);
}

void display(Shader &shader, Camera &camera, s_mode shadowMode = off)
{
    setupShaderUniform(shader, camera, shadowMode);

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

void windowUpdate(Camera &camera, Camera &shadowCamera, Frame &frame0, Frame &frame1, ShadowFrame &shadowFrame, ShadowFrame &pointShadowFrame)
{
    // cout << "DEBUG::MAIN::WU::Shadow position: " << shadowCamera.position.x << " " << shadowCamera.position.y << " " << shadowCamera.position.z << endl;
    if (needUpdateFBO)
    {
        updateFrameVariable(frame0);
        updateFrameVariable(frame1);
        needUpdateFBO = false;
    }

    // render from light view and generate shadow map
    shadowFrame.enable();
    display(shadowShader, shadowCamera, directional);

    // render from light view and generate shadow map
    pointShadowFrame.enable();
    display(pointShadowShader, shadowCamera, point);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // setup depthmap at GL_TEXTURE3
    deferredShader.use();
    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, shadowFrame.depth_tex);
    glActiveTexture(GL_TEXTURE0 + 9);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowFrame.depth_tex);
    // make shadow vp matrix
    mat4 scale_bias(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0);
    deferredShader.setMat4("shadow_sbpv", 
                            scale_bias * ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f) * shadowCamera.getView());
    deferredShader.setVec3("dirtectionalShadowPosition", shadowCamera.getPosition());

    // rander scene to G-buffers.
    glViewport(0, 0, frameWidth, frameHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, frame0.FBO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    display(baseShader, camera);

    // generate SSAO and rander to G-buffers.
    glBindFramebuffer(GL_FRAMEBUFFER, frame1.FBO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    setupShaderUniform(SSAOShader, camera);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, noiseTextureTemp);
    for (unsigned int i = 0; i < 64; ++i)
        SSAOShader.setVec3(("sampleKernel[" + to_string(i) + "]").c_str(), ssaoKernel[i]);
    frame0.draw(SSAOShader);

    // using G-buffers to do deferred shading
    glBindFramebuffer(GL_FRAMEBUFFER, frame0.FBO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    setupShaderUniform(deferredShader, camera);
    frame1.draw(deferredShader);

    // render post-processing effects
    glBindFramebuffer(GL_FRAMEBUFFER, frame1.FBO);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    setupShaderUniform(frameShader, camera);
    frame0.draw(frameShader);

    // FXAA effects
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    setupShaderUniform(FXAAShader, camera);
    frame1.draw(FXAAShader);
    
}

void reshapeResponse(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    frameWidth = width;
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
            cout << "Mouse " << button << " is pressed at (" << x << ", " << y << ")" << endl;
        else if (action == GLFW_RELEASE)
            cout << "Mouse " << button << " is pressed at (" << x << ", " << y << ")" << endl;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
            trackballEnable = true;
        else if (action == GLFW_RELEASE)
            trackballEnable = false;
    }
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
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // load OpenGL function pointer
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    dumpInfo();
    Camera camera = Camera()
                        .withPosition(vec3(4.0f, 1.0f, -1.5f))
                        .withFar(10.0f)
                        .withMoveSpeed(3.0f)
                        .withTheta(180.0f);
    Camera shadowCamera = Camera()
                              .withPosition(directionalShadowPosition)
                              .withNear(0.1f)
                              .withFar(10.0f);
    shadowCamera.setLookAt(vec3(0.542f, -0.141f, -0.422f));
    setGUICameraStatus(camera);

    ShadowFrame shadowFrame(0);
    ShadowFrame pointShadowFrame(1);
    Frame deferredFrame = Frame();
    Frame frame = Frame();
    ImguiPanel imguiPanel = ImguiPanel(INIT_WIDTH, INIT_HEIGHT);
    imguiPanel.initinalize(window);
    initialization(window);

    // register glfw callback functions
    glfwSetFramebufferSizeCallback(window, reshapeResponse);
    glfwSetKeyCallback(window, keyboardResponse);
    glfwSetMouseButtonCallback(window, mouseResponse);

    cout << "DEBUG::MAIN::M::Set up done" << endl;
    // main loop
    float timeDifferent = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // Poll input event
        glfwPollEvents();
        timerUpdate();

        processCameraMove(camera);
        processCameraTrackball(camera, window);
        windowUpdate(camera, shadowCamera, deferredFrame, frame, shadowFrame, pointShadowFrame);
        imguiPanel.setWidth(frameWidth);
        imguiPanel.setHeight(frameHeight);
        imguiPanel.guiMenu(
            camera, 
            blinnPhongEnabled, 
            directionalShadowEnabled,
            pointShadowEnabled,
            normalMappingEnabled, 
            bloomEffectEnabled, 
            effectTestMode, 
            SSAOEnabled, 
            NPREnabled, 
            FXAAEnabled, 
            gBufferMode, 
            cameraPosition, 
            cameraLookAt, 
            directionalShadowPosition,
            pointShadowPosition);
        shadowCamera.setPosition(directionalShadowPosition);
        lights[0].setPosition(pointShadowPosition);

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }

    imguiPanel.menuCleanup();
    // just for compatibiliy purposes
    return 0;
}
