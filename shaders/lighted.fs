#version 460 core

#define NB_POINT_LIGHTS POINT_QTE
#define NB_DIR_LIGHTS   DIR_QTE
#define NB_SPOT_LIGHTS  SPOT_QTE

#define PI 3.14159265359

struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;

    sampler2D albedoMap;
    sampler2D roughnessMap;
    sampler2D metallicMap;
    sampler2D aoMap;

    bool hasAlbedoMap;
    bool hasRoughnessMap;
    bool hasMetallicMap;
    bool hasAOMap;

    sampler2D normalMap;

    bool hasNormalMap;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct SpotLight {
    vec3 direction;
    vec3 position;
    float innerCos;
    float outerCos;
    vec3 color;
    float intensity;
    float radius;
};

struct Skybox {
    samplerCube irradianceMap;
    samplerCube environmentMap;
    sampler2D brdfLUT;

    vec3 background;

    bool hasSkybox;
};

out vec4 FragColor;              // Visible color of the fragment after computation

in vec3 FragPos;                 // World's position of the fragment
in vec3 Normal;                  // Normal of the fragment
in vec3 Tangent;
in vec2 UV;                      // UV value of the fragment (for textures)

uniform Skybox skybox;           // Skybox informations

uniform Material material;       // Material of the fragment

bool PBR_ONLY = false;


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

uniform vec3 viewPos; // Camera position in the world space

/* UNIFORMS FOR DEBUGGING */

uniform int renderingMode;

/**
 * Fresnel-Schlick operator for calculating F value
 * @param cosTheta cosinus of the angle
 * @param F0 refraction index of the material (default is 0.04)
 */
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

/**
 * Fresnel-Schlick operator specified for roughness computation
 * @param cosTheta cosinus of the angle
 * @param F0 refraction index of the material (default is 0.04)
 * @param roughness roughness of the material
 */
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 PointRadiance(PointLight light)
{
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (distance * distance);
    return light.color * attenuation * light.intensity; // Radiance
}

vec3 DirRadiance(DirLight light)
{
    return light.color * light.intensity; // Radiance
}

vec3 SpotRadiance(SpotLight light, vec3 L)
{
    float d = length(light.position - FragPos);
    float cosTheta = dot(normalize(light.direction), -L);

    float angleAtt = clamp((cosTheta - light.outerCos) / (light.innerCos - light.outerCos), 0.0, 1.0);
    float distance = length(light.position - FragPos);
    float distAtt = 1.0 / (distance * distance);
    return light.color * light.intensity * angleAtt * distAtt; // Radiance
}

vec3 PBR(vec3 radiance, vec3 L, vec3 V, vec3 N,
         vec3 albedo, float roughness, float metallic,
         vec3 F0)
{  
    vec3 H = normalize(V + L);

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness); 
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 AmbientPBR(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic, float ao)
{
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = vec3(0.03);
    if(skybox.hasSkybox) irradiance = texture(skybox.irradianceMap, N).rgb;
    else irradiance = skybox.background;
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-V, N);
    
    const float MAX_REFLECTION_LOD = 10.0;
    vec3 prefilteredColor = vec3(0.03);
    if(skybox.hasSkybox) prefilteredColor = textureLod(skybox.environmentMap, R,  roughness * MAX_REFLECTION_LOD).rgb;  
    else prefilteredColor = skybox.background; 
    vec2 envBRDF  = texture(skybox.brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao;
    return ambient;
}

void FullRendering()
{
    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    vec3 V = normalize(viewPos - FragPos);

    // Material selection
    if(material.hasNormalMap) {
        vec3 B = cross(T, N); // Bitangent
        mat3 TBN = mat3(T, B, N);
        vec3 normal = texture(material.normalMap, UV).rgb;
        normal = normal * 2.0 - 1.0;
        N = normalize(TBN * normal);
    }
    
    vec3 albedo = material.albedo;
    if (material.hasAlbedoMap && !PBR_ONLY) albedo = albedo * texture(material.albedoMap, UV).rgb;

    // Gestion de la norme ORM (Occlusion - Roughness - Metallic) ---------------------------------
    float ao = material.ao;
    if (material.hasAOMap && !PBR_ONLY) ao = ao * texture(material.aoMap, UV).r;

    float roughness = material.roughness;
    if (material.hasRoughnessMap && !PBR_ONLY) roughness = roughness * texture(material.roughnessMap, UV).g;

    float metallic = material.metallic;
    if (material.hasMetallicMap && !PBR_ONLY) metallic = metallic * texture(material.metallicMap, UV).b;
    //---------------------------------------------------------------------------------------------

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);

#ifdef POINT_LIGHTS
    for(int i = 0; i < NB_POINT_LIGHTS; ++i) {
        vec3 L = normalize(pointLights[i].position - FragPos); 
        vec3 radiance = PointRadiance(pointLights[i]);   
        Lo += PBR(radiance, L, V, N, albedo, roughness, metallic, F0);
    }
#endif
#ifdef DIR_LIGHTS
    for(int i = 0; i < NB_DIR_LIGHTS; ++i) {
        vec3 L = normalize(dirLights[i].direction); 
        vec3 radiance = DirRadiance(dirLights[i]);     
        Lo += PBR(radiance, L, V, N, albedo, roughness, metallic, F0);
    }
#endif
#ifdef SPOT_LIGHTS
    for(int i = 0; i < NB_SPOT_LIGHTS; ++i) {
        vec3 L = normalize(spotLights[i].position - FragPos); 
        vec3 radiance = SpotRadiance(spotLights[i], L);      
        Lo += PBR(radiance, L, V, N, albedo, roughness, metallic, F0);
    }
#endif

    vec3 ambient = AmbientPBR(N, V, F0, albedo, roughness, metallic, ao);

    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0)); // Tone mapping
    color = pow(color, vec3(1.0/2.2));   // Gama correction
   
    FragColor = vec4(color, 1.0);
}



/*************************************************************************************************
 *                                         MAIN SECTION                                          *
 *************************************************************************************************/

#define M_PBR           0

#define M_NORMALS       1
#define M_TANGENT       2
#define M_UVS           3

#define M_ALBEDO        4
#define M_ROUGHNESS     5
#define M_METALLIC      6
#define M_AO            7

#define M_LIGHT_PBR     8
#define M_AMBIENT_PBR   9

void main() {
    switch (renderingMode)
    {
    case M_PBR:
        FullRendering();
        break;

    case M_NORMALS:
        FragColor = vec4((normalize(Normal) + vec3(1.0)) * 0.5, 1.0);
        break;

    case M_TANGENT:
        FragColor = vec4((normalize(Tangent) + vec3(1.0)) * 0.5, 1.0);
        break;

    case M_UVS:
        FragColor = vec4(UV, 0.0, 1.0);
        break;

    case M_ALBEDO:
        vec3 albedo = material.albedo;
        if (material.hasAlbedoMap && !PBR_ONLY) albedo = albedo * texture(material.albedoMap, UV).rgb;
        FragColor = vec4(albedo, 1.0);
        break;

    case M_ROUGHNESS:
        float roughness = material.roughness;
        if (material.hasRoughnessMap && !PBR_ONLY) roughness = roughness * texture(material.roughnessMap, UV).g;
        FragColor = vec4(vec3(roughness), 1.0);
        break;

    case M_METALLIC:
        float metallic = material.metallic;
        if (material.hasMetallicMap && !PBR_ONLY) metallic = metallic * texture(material.metallicMap, UV).b;
        FragColor = vec4(vec3(metallic), 1.0);
        break;

    case M_AO:
        float ao = material.ao;
        if (material.hasAOMap && !PBR_ONLY) ao = ao * texture(material.aoMap, UV).r;
        FragColor = vec4(vec3(ao), 1.0);
        break;
    }
}