#define STB_IMAGE_IMPLEMENTATION

#include "glad.h"
#include "glfw3.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"
#include <vector>

#include "shaderMain.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

// initate some poorly named data
float moveX, moveY;
float scale;

int windowWidth = 800, windowHeight = 600;
float deltaTime = 0.0f, lastFrame   = 0.0f;
float pitch     = 0.0f, yaw         = -90.0f;
float lastX = windowWidth/2, lastY = windowHeight/2;

const float gravityAcc  = 9.8f;
const float mouseSensitivity = 0.1f;
bool mouseStart = true;
bool onLand     = true;
bool jumped     = false;
float jumpFrame = 0.0f, airTime = 0.0f;

const float playerHeight = 1.0f;
const float playerSpeed  = 2.0f;
glm::vec3 cameraPos = glm::vec3(50.0f, playerHeight, 50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float plane[] =
{
//       positions        tex coords
    1.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
    1.0f,  0.0f,  1.0f,  0.0f, 1.0f,
};

std::vector<float> vertices;

void constructPlane();
void processInput(GLFWwindow *window);
void framebufferSize(GLFWwindow *window, int width, int height);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "el gatos", NULL, NULL);
    if (!window)
    {
        std::cout << "nooo" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);

    // set to 0 to disable vsync
    glfwSwapInterval(1);
    gladLoadGL();

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferSize);

    // create and compile vertex and fragment shader
    Shader shader ("../assets/shaders/vertex.glsl", "../assets/shaders/fragment.glsl");

    // !---- work with our textures ----!
    uint32_t texture1;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    // loading and generating the texture
    int width, height, numChannels;
    unsigned char* data = stbi_load("../assets/images/elgato.png", &width, &height, &numChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "bruh there's an error loading that image" << std::endl;
    }

    // finally free the image memory
    stbi_image_free(data);

    shader.Use();
    // specify the texture unit
    shader.setInt("outTexture", 0);

    // creates a plane stored in the vertices vector object, a pointer at the address of vertices[0] would be array like (float*)
    constructPlane();
    float* vertexArray = &vertices[0];

    // !--- create buffers for our vertices and indices and a vertex array object ---!
    VAO VAO1;
    VBO VBO1 (vertexArray, vertices.size() * sizeof(float));
    // EBO elementBuffer (indices, sizeof(indices));

    // pointing to the 3D position vertex attributes
    shader.vertexAttribute(0, 3, 5*sizeof(float), (void*)0);

    // pointing to the texture 2D position vertex attributes
    shader.vertexAttribute(2, 2, 5*sizeof(float), (void*)(3*sizeof(float)));

    VAO1.Unbind();
    VBO1.Unbind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // uncomment to see fps
        // std::cout << 1/deltaTime << std::endl;

        processInput(window);

        glClearColor(0.5, 0.9, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        shader.Use();

        // load in the vertices
        glm::mat4 model = glm::mat4(1.0f);

        // the view matrix, from world space coordinates to view space from the camera
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        // the perspective projection matrix, view space to the clip space
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(90.0f), (float)windowWidth/(float)windowHeight, 0.01f, 100.0f);

        VAO1.Bind();

        // bring the combined transformation matrix into a uniform
        shader.setMat4("transform", glm::value_ptr(projection*view*model));

        // going to modify the vertices and switch to triangle strip to improve performance
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() * sizeof(float));

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

std::vector<float> nearestPlane(glm::vec3 pos)
{
    // the integer casting works as floor() so we can obtain the position of the closest vertex to the bottom left
    float lowestVertex[] = {(int)pos.x, (int)pos.z};
    std::vector<float> plane = {lowestVertex[0]    , sin(lowestVertex[0] / 2)        + cos(lowestVertex[1] / 2)       , lowestVertex[1],
                                lowestVertex[0] + 1, sin(lowestVertex[0] / 2 + 0.5f) + cos(lowestVertex[1] / 2)       , lowestVertex[1],
                                lowestVertex[0]    , sin(lowestVertex[0] / 2)        + cos(lowestVertex[1] / 2 + 0.5f), lowestVertex[1] + 1,
                                lowestVertex[0] + 1, sin(lowestVertex[0] / 2 + 0.5f) + cos(lowestVertex[1] / 2 + 0.5f), lowestVertex[1] + 1};
    return plane;
}

// P is the point of collision
float P;
bool colliding(glm::vec3 cameraPos)
{
    std::vector<float> plane = nearestPlane(cameraPos);

    // an estimation of the height of the surface from the normal vector generated by 3 points on the nearest plane
    float normalVector[] = {(plane[1]-plane[7]) * (plane[2]-plane[5]) - (plane[2]-plane[8]) * (plane[1]-plane[4]),
                            (plane[2]-plane[8]) * (plane[0]-plane[3]) - (plane[0]-plane[6]) * (plane[2]-plane[5]),
                            (plane[0]-plane[6]) * (plane[1]-plane[4]) - (plane[1]-plane[7]) * (plane[0]-plane[3])};

    P = -(normalVector[0] * (cameraPos.x - plane[0]) + normalVector[2] * (cameraPos.z - plane[2]) - normalVector[1] * plane[1]) / normalVector[1];

    return (cameraPos.y - playerHeight < P);
}

void doCollisions(glm::vec3 &cameraPos)
{
    if (colliding(cameraPos))
    {
        // 0.03 margin added to reduce collision bugginess (and allow for jumping)
        cameraPos.y = P + playerHeight - 0.03;
        onLand = true;
    }
}

void gravity(GLFWwindow* window, glm::vec3 &cameraPos)
{
    if (colliding(cameraPos) == false)
    {
        if (onLand)
        {
            jumpFrame = glfwGetTime();
            onLand = false;
        }
    }
    else
    {
        onLand = true;
        jumped = false;
        airTime = 0.0f;
        jumpFrame = 0.0f;
    }
    if (onLand && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPos.y += 0.03;
        jumpFrame = glfwGetTime();
        jumped = true;
        onLand = false;
    } 
    if (!onLand)
    {
        if (jumped)
        {
            cameraPos.y += (4 * exp(-airTime * airTime)) * deltaTime;
        }
        airTime = glfwGetTime() - jumpFrame;
    }

    // void respawn
    if (cameraPos.y < -10.0f)
    {
        cameraPos = glm::vec3(5.0f, playerHeight, 5.0f);
        airTime = 0.0f;
    }

    cameraPos.y -= (airTime * gravityAcc) * deltaTime;
    doCollisions(cameraPos);
}

void moveCamera(GLFWwindow *window, glm::vec3 &cameraPos, float speed)
{
    glm::vec3 cameraForward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += speed * cameraForward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= speed * cameraForward;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= speed * glm::normalize(glm::cross(cameraForward, cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += speed * glm::normalize(glm::cross(cameraForward, cameraUp));
    }
    
    gravity(window, cameraPos);
}

void rotateCamera(GLFWwindow *window, double xPos, double yPos)
{
    if (mouseStart)
    {
        lastX = xPos;
        lastY = yPos;
        mouseStart = false;
    }

    float xOffset = (xPos - lastX) * mouseSensitivity;
    float yOffset = (yPos - lastY) * mouseSensitivity;

    lastX = xPos;
    lastY = yPos; 

    pitch -= yOffset;
    yaw   += xOffset;

    // clamping the pitch to avoid buggy movement
    if (pitch > 89)
    {
        pitch = 89;
    }
    if (pitch < -89)
    {
        pitch = -89;
    }

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = playerSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed = 5.0f * deltaTime;
    }

    moveCamera(window, cameraPos, cameraSpeed);
    glfwSetCursorPosCallback(window, rotateCamera);
}

// we're just making a temporary plane that we shift all around and append each to our vertices to generate a large plane
// may be catastrophically terrible not sure yet since im not good at c++
float planeTemp[sizeof(plane) / sizeof(float)];
void constructPlane()
{
    memcpy(planeTemp, plane, sizeof(plane));
    for (unsigned int i = 0; i < 10000; i++)
    {
        // for a square with 6 vertices, very intuitive
        for (unsigned int x = 0; x < 6; x++)
        {
            planeTemp[5*x]   = plane[5*x]   + i % 100;
            planeTemp[1+5*x] = sin(planeTemp[5*x] / 2) + cos(planeTemp[2+5*x] / 2);
            planeTemp[2+5*x] = plane[2+5*x] + (int)(i / 100);
        }
        vertices.insert(vertices.end(), planeTemp, planeTemp + sizeof(plane) / sizeof(float));
    }
}

void framebufferSize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}