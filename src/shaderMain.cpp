#include "shaderMain.h"

void readFile(std::string currentLine, std::ifstream& file, std::string& code)
{
    if (file.is_open())
    {
        // adding every line in a file to a code string
        while (std::getline(file, currentLine))
        {
            code += currentLine + '\n';
        }
    }
    else
    {
        std::cout << "file failed to open" << std::endl;
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    vertexFile.open(vertexPath);
    fragmentFile.open(fragmentPath);

    readFile(line, vertexFile, vertexCode);
    readFile(line, fragmentFile, fragmentCode);

    vertexSource = vertexCode.c_str();
    fragmentSource = fragmentCode.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

    glLinkProgram(ID);

    getDumbLittleErrors();
}

void Shader::Use()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::vertexAttribute(int location, int size, GLsizei stride, const void* offset)
{
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(location);
}

void Shader::setInt(const char* name, int v0)
{
    glUniform1i(glGetUniformLocation(ID, name), v0);
}
void Shader::setFloat(const char* name, float v0)
{
    glUniform1f(glGetUniformLocation(ID, name), v0);
}
void Shader::setMat4(const char* name, const GLfloat* matrixValue)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, matrixValue);
}