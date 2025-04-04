#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 UV;

uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float ambientStrength;
uniform bool uniformColor;

// 0 = Visualisation des couleurs
// 1 = Visualisation de la normale
// 2 = Visualisation de la UV map
uniform int displayMode;

void main()
{
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

    // Diffuse color
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Distance shading
    float alpha = 4.0;
    float invDist = alpha / length(lightPos - FragPos);

    // Result color
    vec3 result = (ambient + diffuse) * color * invDist;
    FragColor = vec4(result, 1.0);
}