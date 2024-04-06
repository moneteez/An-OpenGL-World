#pragma once

#include "glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    std::string line;
    uint32_t ID;

    Shader (const char* vertexPath, const char* fragmentPath);

    void Use();
    void Delete();
    void vertexAttribute(int location, int size, GLsizei stride, const void* offset);
    
    // information to uniforms
    void setInt(const char* name, int v0);
    void setFloat(const char* name, float v0);
    void setMat4(const char* name, const GLfloat* matrixValue);
private:
    const char *vertexSource, *fragmentSource;
    std::string vertexCode, fragmentCode;

    // initializing vertex and fragment file references
    // ifstream objects have a deleted copy constructor which doesn't let you set a value to them directly
    std::ifstream vertexGLSL, fragmentGLSL;
    std::ifstream &vertexFile = vertexGLSL, &fragmentFile = fragmentGLSL;

    uint32_t vertexShader, fragmentShader;

    int success;
    char log[512];
    void getDumbLittleErrors()
    {   
        // vertex shader error check
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, log);
            std::cout << "vertex shader failed whyyyyyy\n" << log << std::endl;
        }
        // fragment shader error check
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, log);
            std::cout << "fragment shader failed whyyyyyy\n" << log << std::endl;
        }
        // shader program error check
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, log);
            std::cout << "the shader program failed to link whyyyyyy\n" << log << std::endl;
        }
    }
};