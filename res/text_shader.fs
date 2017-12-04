#version 330 core
// vim: set ft=glsl:

out vec4 fragColor;

in vec2 fsTexCoord;

uniform sampler2D texture1;

void main()
{
    fragColor = texture(texture1, fsTexCoord);
}


