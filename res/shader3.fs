#version 330 core

// vim: ft=glsl:

out vec4 FragColor; /* Output of fragment shader */

in vec4 vertexColor; /* Input variable from vertex shader */


void main()
{
    FragColor = vertexColor;
} 


