#ifndef COMMON_HPP
#define COMMON_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

// #ifndef TINYOBJLOADER_IMPLEMENTATION
// #define TINYOBJLOADER_IMPLEMENTATION
// #include "TinyOBJ/tiny_obj_loader.h"
// #endif

// #include "utils/glutils.hpp"
#include "utils/timer.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define GLM_FORCE_SWIZZLE
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FILEPATH__(x) ((std::string(__FILE__).substr(0, std::string(__FILE__).rfind('/'))+(x)).c_str())

#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))

#define INIT_WIDTH 1600
#define INIT_HEIGHT 900
#define INIT_VIEWPORT_X 0
#define INIT_VIEWPORT_Y 0
#define MAX_BONE_INFLUENCE 4

using namespace glm;
using namespace std;

// Print OpenGL context related information.
extern void dumpInfo(void);

extern void shaderLog(GLuint shader);

extern void printGLError();

#endif
