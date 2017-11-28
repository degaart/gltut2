#version 330 core

// vim: ft=glsl:

out vec4 FragColor; /* Output of fragment shader */

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;     /* accessed with glBindTexture */
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
} 


