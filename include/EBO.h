#pragma once

#include "glad.h"
#include <iostream>

class EBO
{
public:
    uint32_t ID;

    EBO (uint32_t* indices, GLsizeiptr size)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    }

    void Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    }
    void Unbind()  
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void Delete()
    {
        glDeleteBuffers(1, &ID);
    }
};