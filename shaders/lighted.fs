#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 direction;
    vec3 position;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 UV;

uniform Material material;
uniform PointLight light;

uniform vec3 viewPos;

void main()
{
    // Ambient color (currently useless)
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Phong light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // Distance shading
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    // Result color
    vec3 result = (ambient + diffuse + specular) * attenuation;
    FragColor = vec4(result, 1.0);
}

/**
 * Compute Directional light influence on the current fragment
 */
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = (-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient  = light.ambient;  // * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse;  // * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular; // * spec * vec3(texture(material.specular, TexCoords));
    
    return (ambient + diffuse + specular);
}