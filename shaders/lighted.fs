#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float ambientStrength;

void main()
{
    // Ambient color (currently useless)
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse color
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Result color
    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}