cmake_minimum_required(VERSION 3.28.2)

project(graphics_dingusness)

find_package(OpenGL REQUIRED)

include_directories("../include/glad" "../include/glfw" "../include/stb" "../include" "../include/glm" "../include/glm/gtc")
add_executable( graphics_dingusness 
                ../src/main.cpp
                ../src/glad.c 
                ../src/shaderMain.cpp   

                ../include/glad/glad.h
                ../include/glfw/glfw3.h
                ../include/stb/stb_image.h
                ../include/glm/glm.hpp
                ../include/glm/gtc/matrix_transform.hpp
                ../include/glm/gtc/type_ptr.hpp
                
                ../include/shaderMain.h 
                ../include/VAO.h
                ../include/VBO.h
                ../include/EBO.h)

target_link_libraries(graphics_dingusness  
                    glfw3
                    OpenGL::GL 
                    ${CMAKE_DL_LIBS} )

