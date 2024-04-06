#pragma once

#include "glad.h"
#include <iostream>

class VBO
{
public:
    uint32_t ID;

    VBO (float* vertices, GLsizeiptr size)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    void Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
    }
    void Unbind()  
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void Delete()
    {
        glDeleteBuffers(1, &ID);
    }
};