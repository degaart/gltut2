#version 330 core

// vim: ft=glsl:

out vec4 FragColor; /* Output of fragment shader */

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
} 


