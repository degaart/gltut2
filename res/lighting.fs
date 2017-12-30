#version 330 core

// vim: ft=glsl:
out vec4 FragColor;
  
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    /* Ambient is the same texture as diffuse */
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outerCutoff;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec3 result;

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // Light direction
    vec3 lightDir = normalize(light.position - FragPos);

    // Theta angle for flashlight
    float theta = dot(lightDir, normalize(-light.direction));

    // Epsilon for flashlight smoothing
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // Leave ambient alone so it's always applied
    diffuse *= intensity;
    specular *= intensity;

    result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}


