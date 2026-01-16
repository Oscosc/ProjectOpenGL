#version 460 core

#define NB_POINT_LIGHTS POINT_QTE
#define NB_DIR_LIGHTS   DIR_QTE
#define NB_SPOT_LIGHTS  SPOT_QTE

#define PI 3.14159265359


/*************************************************************************************************
 *                                      STRUCTURES SECTION                                       *
 *************************************************************************************************/

/**
 * Material as defined in gLTF documentation :
 * https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation
 */
struct Material {
    vec3 color;
    float roughness;
    float metallic;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
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
};


/*************************************************************************************************
 *                                      PARAMETERS SECTION                                       *
 *************************************************************************************************/

out vec4 FragColor;         // Visible color of the fragment after computation

in vec3 FragPos;            // World's position of the fragment
in vec3 Normal;             // Normal of the fragment
in vec3 UV;                 // UV value of the fragment (for textures)

uniform Material material;  // Material of the fragment

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
 *                               BRDF/MICROFACETS FUNCTIONS SECTION                              *
 *************************************************************************************************/

float DistributionTrowbridgeReitzGGX(vec3 N, vec3 H, float alpha2)
{
    float NdotH = max(dot(N, H), 0.0);
    float denominator = (NdotH * NdotH * (alpha2 - 1.0) + 1.0);

    return alpha2 / (PI * denominator * denominator);
}

float SeparatedSmithJoint(float NdotX, float alpha2)
{
    float denominator = abs(NdotX) + sqrt(alpha2 + (1.0 - alpha2) * NdotX * NdotX);

    return (2.0 * abs(NdotX)) / denominator;
}

vec3 FresnelSchlick(float HdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}

/**
 * @brief Main microfacets/BRDF function. Describe how light is reflected based on parameters.
 * https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation
 *
 * @param N Surface normal vector (world space)
 * @param V Normalized view vector, from fragment position to camera position (world space)
 * @param L Normalized light vector, from fragment position to light position (world space)
 * @param color Color of the fragment, part of the material (range 0 -> 1 for each element)
 * @param roughness Roughness of the fragment, part of the material (range 0 -> 1)
 * @param metallic Metallic level of the fragment, part of the material (range 0 -> 1)
 */
vec3 MicrofacetsBRDF(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float roughness, float metallic)
{
    // Check for "no light angle"
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    if (NdotL <= 0.0 || NdotV <= 0.0) return vec3(0.0);

    // Material "preparation"
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 cDiff = mix(albedo, vec3(0.0), metallic);

    // Specular
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    
    float D = DistributionTrowbridgeReitzGGX(N, H, alpha2);
    float G = SeparatedSmithJoint(NdotL, alpha2) * SeparatedSmithJoint(NdotV, alpha2);
    vec3 F  = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotL * NdotV;
    vec3 specular = numerator / max(denominator, 0.0001);

    // Diffuse
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);
    
    vec3 diffuse = kD * cDiff / PI;

    // Final color
    return (diffuse + specular) * radiance * NdotL;
}


/*************************************************************************************************
 *                                         MAIN SECTION                                          *
 *************************************************************************************************/

void main()
{
    vec3 accumulatedColor = vec3(0.0, 0.0, 0.0);

    vec3 V = normalize(viewPos - FragPos);
    vec3 N = normalize(Normal);
    vec3 albedo = material.color;

#ifdef POINT_LIGHTS

    // phase 1: Point lights
    for(int i = 0; i < NB_POINT_LIGHTS; i++) {
        vec3 L = normalize(pointLights[i].position - FragPos);
        float d = length(pointLights[i].position - FragPos);

        float attenuation = 1.0 / (d * d);
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        accumulatedColor += MicrofacetsBRDF(N, V, L, radiance, albedo, material.roughness, material.metallic);
    }

#endif

#ifdef DIR_LIGHTS

    // phase 2: Directional lights
    for(int i = 0; i < NB_DIR_LIGHTS; i++) {
        vec3 L = normalize(-dirLights[i].direction);

        // No attenuation
        vec3 radiance = dirLights[i].color * dirLights[i].intensity;


        accumulatedColor += MicrofacetsBRDF(N, V, L, radiance, albedo, material.roughness, material.metallic);
    }

#endif

#ifdef SPOT_LIGHTS

    // phase 3: Spot lights
    for(int i = 0; i < NB_SPOT_LIGHTS; i++) {
        vec3 L = normalize(spotLights[i].position - FragPos);
        float d = length(spotLights[i].position - FragPos);
        float cosTheta = dot(normalize(spotLights[i].direction), -L);

        float angleAtt = clamp((cosTheta - spotLights[i].outerCos) /
            (spotLights[i].innerCos - spotLights[i].outerCos), 0.0, 1.0);
        float distAtt = 1.0 / (d * d);
        vec3 radiance = spotLights[i].color * spotLights[i].intensity * angleAtt * distAtt;

        accumulatedColor += MicrofacetsBRDF(N, V, L, radiance, albedo, material.roughness, material.metallic);
    }

#endif

    // Tone mapping (eq. to normalization)
    vec3 toneMapping = accumulatedColor / (accumulatedColor + vec3(1.0));

    // Gamma correction
    vec3 correctedGamma = pow(accumulatedColor, vec3(1.0 / 2.2));

    FragColor = vec4(correctedGamma, 1.0);
}