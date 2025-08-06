#version 330 core

#define NB_POINT_LIGHTS POINT_QTE
#define NB_DIR_LIGHTS   DIR_QTE
#define NB_SPOT_LIGHTS  SPOT_QTE

#define CONSTANT 1.0
#define LINEAR 0.09
#define QUADRATIC 0.032


/*************************************************************************************************
 *                                      STRUCTURES SECTION                                       *
 *************************************************************************************************/

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


/*************************************************************************************************
 *                                      PARAMETERS SECTION                                       *
 *************************************************************************************************/

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 UV;

uniform Material material;

#if NB_POINT_LIGHTS > 0
    #define POINT_LIGHTS
    uniform PointLight pointLights[NB_POINT_LIGHTS];
#endif

#if NB_DIR_LIGHTS > 0
    #define DIR_LIGHTS
    uniform DirLight dirLights[NB_DIR_LIGHTS];
#endif

#if NB_SPOT_LIGHTS > 0
    #define SPOT_LIGHTS
    uniform SpotLight spotLights[NB_SPOT_LIGHTS];
#endif

uniform vec3 viewPos;


/*************************************************************************************************
 *                                       FUNCTIONS SECTION                                       *
 *************************************************************************************************/

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance));  

    // combine results
    vec3 ambient  = light.ambient * material.ambient;          // * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse * diff * material.diffuse;   // * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specular; // * vec3(texture(material.specular, TexCoords));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = (-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient  = light.ambient * material.ambient;          // * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse * diff * material.diffuse;   // * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specular; // * vec3(texture(material.specular, TexCoords));
    
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * material.ambient;           // * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * material.diffuse;    // * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specular; // * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}


/*************************************************************************************************
 *                                         MAIN SECTION                                          *
 *************************************************************************************************/

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0, 0.0, 0.0);

#ifdef POINT_LIGHTS
    // phase 1: Point lights
    for(int i = 0; i < NB_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
#endif

#ifdef DIR_LIGHTS
    // phase 2: Directional lighting
    for(int i = 0; i < NB_DIR_LIGHTS; i++)
        result += CalcDirLight(dirLights[i], norm, viewDir);
#endif

#ifdef SPOT_LIGHTS
    // phase 3: Spot light
    for(int i = 0; i < NB_SPOT_LIGHTS; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);    
#endif

    FragColor = vec4(result, 1.0);
}