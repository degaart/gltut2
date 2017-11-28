#version 330 core

// vim: ft=glsl:

out vec4 FragColor; /* Output of fragment shader */

uniform vec4 ourColor;  /* Set from opengl code */

void main()
{
    FragColor = ourColor;
} 


