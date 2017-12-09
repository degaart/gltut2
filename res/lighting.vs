#version 330 core

// vim: ft=glsl:

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    /*
     * Get fragment position in world space (position * model)
     * To calculate diffuse color
     */
    FragPos = vec3(model * vec4(aPos, 1.0));

    /* Forward normal to fragment shader */
    Normal = aNormal;
} 


