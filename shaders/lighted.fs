#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 UV;

uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float specularStrength;

// Obsolete
uniform bool uniformColor;

// 0 = Visualisation des couleurs
// 1 = Visualisation de la normale
// 2 = Visualisation de la UV map
uniform int displayMode;

void main()
{
    // For debug visualizations
    if(uniformColor) {
        FragColor = vec4(color, 1.0f);
        return;
    }

    if(displayMode == 1) {
        FragColor = vec4(Normal * 0.5 + 0.5, 1.0);
        return;
    }

    if(displayMode == 2) {
        FragColor = vec4(UV, 1.0);
        return;
    }

    // Ambient color (currently useless)
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Phong light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = (-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    // Distance shading
    float alpha = 4.0;
    float invDist = alpha / length(lightPos - FragPos);

    // Result color
    vec3 result = (ambient + diffuse + specular) * color * invDist;
    FragColor = vec4(result, 1.0);
}