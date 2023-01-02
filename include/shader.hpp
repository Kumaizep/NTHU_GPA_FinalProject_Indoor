#ifndef SHADER_HPP
#define SHADER_HPP

#include "GLM/fwd.hpp"
#include "common.hpp"

class Shader
{
public:
    GLuint program;
    Shader(const char* vertexPath, const char* fragmentPath);

    // activate the shader
    // ------------------------------------------------------------------------
    void use() ;

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setInt(const GLchar* name, int value);
    void setBool(const GLchar* name, bool value);
    void setFloat(const GLchar* name, float value);
    void setVec2(const GLchar* name, float x, float y);
    void setVec3(const GLchar* name, float x, float y, float z);
    void setVec3(const GLchar* name, vec3 vec);
    void setMat4(const GLchar* name, const mat4 &mat);

private:
    char** loadShaderSource(const char* file);

    void freeShaderSource(char** srcp);
};

#endif
