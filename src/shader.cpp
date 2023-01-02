#include "shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    glViewport(INIT_VIEWPORT_X, INIT_VIEWPORT_Y, INIT_WIDTH, INIT_HEIGHT);
    glClearColor(0.0f, 0.3f, 0.0f, 1.00f);
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Create Shader Program
    program = glCreateProgram();

    // Create customize shader by tell openGL specify shader type
    char **vertexShaderSource = loadShaderSource(vertexPath);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    freeShaderSource(vertexShaderSource);
    glCompileShader(vertexShader);
    shaderLog(vertexShader);

    char **fragmentShaderSource = loadShaderSource(fragmentPath);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
    freeShaderSource(fragmentShaderSource);
    glCompileShader(fragmentShader);
    shaderLog(fragmentShader);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    if (geometryPath)
    {
        char **geometryShaderSource = loadShaderSource(geometryPath);
        GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, geometryShaderSource, NULL);
        freeShaderSource(geometryShaderSource);
        glCompileShader(geometryShader);
        shaderLog(geometryShader);
        glAttachShader(program, geometryShader);
    }

    glLinkProgram(program);

    // Tell OpenGL to use this shader program now
    glUseProgram(program);
}
// activate the shader
// ------------------------------------------------------------------------
void Shader::use() 
{ 
    glUseProgram(program); 
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setInt(const GLchar* name, int value)
{ 
    glUniform1i(glGetUniformLocation(program, name), value); 
}

void Shader::setBool(const GLchar* name, bool value)
{ 
    glUniform1i(glGetUniformLocation(program, name), value); 
}

void Shader::setFloat(const GLchar* name, float value)
{ 
    glUniform1f(glGetUniformLocation(program, name), value); 
}

void Shader::setVec2(const GLchar* name, float x, float y)
{ 
    glUniform2f(glGetUniformLocation(program, name), x, y); 
}

void Shader::setVec3(const GLchar* name, float x, float y, float z)
{ 
    glUniform3f(glGetUniformLocation(program, name), x, y, z); 
}

void Shader::setVec3(const GLchar* name, vec3 vec)
{ 
    glUniform3f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z); 
}

void Shader::setMat4(const GLchar* name, const mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, &mat[0][0]);
}

char** Shader::loadShaderSource(const char* file)
{
    FILE* fp = fopen(file, "rb");
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *src = new char[sz + 1];
    auto result = fread(src, sizeof(char), sz, fp);
    src[sz] = '\0';
    char **srcp = new char*[1];
    srcp[0] = src;
    return srcp;
}

void Shader::freeShaderSource(char** srcp)
{
    delete srcp[0];
    delete srcp;
}
