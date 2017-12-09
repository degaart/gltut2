#version 330 core

// vim: ft=glsl:
out vec4 FragColor;
  
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    /* Ambient lighting */
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    /* 
     * Diffuse lighting
     * Direction vector between light source and fragment's position
     */
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    /* 
     * Calculate diffuse by using angle between the normal and the light direction
     * The greater the angle between the two vectors, the darker the fragment is
     */
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    /* Lighting result */
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}


