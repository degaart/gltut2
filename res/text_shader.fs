#version 330 core
// vim: set ft=glsl:

out vec4 fragColor;

in vec2 fsTexCoord;

uniform sampler2D texture1;

void main()
{
    vec4 col = texture(texture1, fsTexCoord);
    fragColor = vec4(col.r, col.r, col.r, 1.0f);
}


