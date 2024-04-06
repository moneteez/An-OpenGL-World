#version 330 core

layout (location = 0) in vec3 vertPos;  
layout (location = 1) in vec3 vertColor;  
layout (location = 2) in vec2 vertTexCoord;

out vec3 uv;  
out vec2 texCoord;  

uniform mat4 transform;

void main()  
{  
    gl_Position = transform * vec4(vertPos, 1.0f);   
    uv = (gl_Position.xyz/2.0)+0.5;
    texCoord = vertTexCoord;  
}
