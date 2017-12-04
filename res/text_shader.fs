#version 330 core

// vim: ft=glsl:

out vec4 fragColor; /* Output of fragment shader */
in vec2 fsTexCoord;
uniform sampler2D texture1;

void main()
{
    //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    fragColor = texture(texture1, fsTexCoord);
} 


