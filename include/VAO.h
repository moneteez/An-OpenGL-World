#pragma once

#include "glad.h"
#include <iostream>

class VAO
{
public:
    uint32_t ID;

    VAO ()
    {
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
    }

    void Bind()
    {
        glBindVertexArray(ID);
    }
    void Unbind()
    {
        glBindVertexArray(0);
    }
    void Delete()
    {
        glDeleteVertexArrays(1, &ID);
    }
};