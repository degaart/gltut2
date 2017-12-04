#version 330 core

// vim: set ft=glsl:

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 fsTexCoord;

void main()
{
    gl_Position = vec4(pos, 1.0);
    fsTexCoord = texCoord;
}
