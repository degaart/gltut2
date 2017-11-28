#version 330 core

// vim: ft=glsl:

out vec4 FragColor; /* Output of fragment shader */

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;     /* accessed with glBindTexture */
uniform sampler2D texture2;

uniform float texMix;

void main()
{
    vec2 flippedCoord = vec2(-TexCoord.x, TexCoord.y);

    vec4 color1 = texture(texture1, TexCoord);
    vec4 color2 = texture(texture2, flippedCoord);
    FragColor = mix(color1, color2, texMix * color2.a);
} 


