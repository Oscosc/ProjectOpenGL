#version 460 core

#define NB_POINT_LIGHTS POINT_QTE
#define NB_DIR_LIGHTS   DIR_QTE
#define NB_SPOT_LIGHTS  SPOT_QTE

#define PI 3.14159265359

#define M_PBR           0
#define M_NORMALS       1
#define M_TANGENT       2
#define M_UVS           3
#define M_ALBEDO        4
#define M_ROUGHNESS     5
#define M_METALLIC      6
#define M_AO            7
#define M_HEIGHT        8
#define M_SHADOW        9


/*************************************************************************************************
 *                                      STRUCTURES SECTION                                       *
 *************************************************************************************************/

/**
 * @brief Defines the physical and visual properties of the surface being rendered.
 *
 * Visual Effect: This determines what the object looks like (e.g., plastic, rusted metal,
 * smooth mirror).
 *
 * It combines base color (albedo), micro-surface irregularities (roughness), reflectivity type 
 * (metallic), fake ambient shadowing (AO), and fake physical depth (height) using either uniform 
 * values or specific textures.
 */
struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;
    float height;

    sampler2D albedoMap;
    sampler2D roughnessMap;
    sampler2D metallicMap;
    sampler2D aoMap;
    sampler2D heightMap;
    sampler2D normalMap;

    bool hasAlbedoMap;
    bool hasRoughnessMap;
    bool hasMetallicMap;
    bool hasAOMap;
    bool hasHeightMap;
    bool hasNormalMap;
};

/**
 * @brief Represents a localized light source radiating equally in all directions.
 *
 * Visual Effect: Creates a spherical area of illumination that smoothly fades out as the 
 * distance from the 'position' increases (attenuation). Useful for simulating light bulbs, 
 * torches, or glowing orbs.
 */
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

/**
 * @brief Represents an infinitely far light source with parallel light rays.
 *
 * Visual Effect: Illuminates all objects in the scene equally from a specific direction without 
 * fading over distance. Commonly used to simulate the Sun or the Moon.
 */
struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

/**
 * @brief Represents a localized light source emitting light within a specific cone angle.
 *
 * Visual Effect: Simulates flashlights or stage spotlights. It creates a circular pool of light 
 * that fades both with distance and towards the edges of the cone (soft edges between innerCos 
 * and outerCos).
 */
struct SpotLight {
    vec3 direction;
    vec3 position;
    float innerCos;
    float outerCos;
    vec3 color;
    float intensity;
    float radius;
};

/**
 * @brief Holds environmental textures used for Image-Based Lighting (IBL) and background 
 * rendering.
 *
 * Visual Effect: Provides highly realistic ambient lighting by wrapping the scene in a 360-degree 
 * image. It applies global indirect lighting (irradiance) and reflects the surrounding 
 * environment onto shiny/metallic surfaces (environmentMap and brdfLUT).
 */
struct Skybox {
    samplerCube irradianceMap;
    samplerCube environmentMap;
    sampler2D brdfLUT;

    vec3 background;

    bool hasSkybox;
    float exposure;
};


/*************************************************************************************************
 *                                      PARAMETERS SECTION                                       *
 *************************************************************************************************/

out vec4 FragColor;


in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec2 UV;

in vec3 TangentFragPos;
in vec3 TangentViewPos;

in vec4 FragPosLightSpace;


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

uniform Skybox    skybox;
uniform Material  material;
uniform vec3      viewPos;
uniform sampler2D shadowMap;
uniform int       renderingMode;
uniform bool      toneMappingOn;
uniform bool      gammaCorrectionOn;


/*************************************************************************************************
 *                                       FUNCTIONS SECTION                                       *
 *************************************************************************************************/

/**
 * @brief Calculates the Fresnel effect for a surface using the Schlick approximation.
 * 
 * Visual Effect: Makes surfaces appear much more reflective when viewed at grazing, shallow 
 * angles compared to when viewed directly head-on. Essential for realistic dielectric materials 
 * (like glass or water).
 * 
 * @param cosTheta The cosine of the angle between the view direction and the half-vector.
 * @param F0 The base reflectivity of the surface at normal incidence (usually 0.04 for
 * dielectrics).
 * 
 * @return The ratio of light that is reflected (specular fraction) as a vec3 color.
 */
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


/**
 * @brief Calculates the Fresnel effect but dampens it based on the surface's roughness.
 * 
 * Visual Effect: Prevents rough materials from appearing too perfectly reflective at grazing 
 * angles. A matte surface will have a much softer and less pronounced edge reflection than a 
 * perfectly smooth one.
 * 
 * @param cosTheta The cosine of the angle between the surface normal and the view direction.
 * @param F0 The base reflectivity of the surface at normal incidence.
 * @param roughness The roughness of the material (0.0 = smooth, 1.0 = rough).
 *
 * @return The modified ratio of reflected light accounting for roughness.
 */
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

/**
 * @brief Computes the Normal Distribution Function (NDF) using the Trowbridge-Reitz GGX model.
 * 
 * Visual Effect: Determines the size, shape, and sharpness of specular highlights (light 
 * reflections). Low roughness creates small, blindingly sharp highlights, while high roughness 
 * spreads the highlight into a wide, dull glow.
 * 
 * @param N The normalized surface normal vector.
 * @param H The normalized half-vector halfway between the view direction and light direction.
 * @param roughness The roughness of the material.
 *
 * @return The statistical proportion of microfacets perfectly aligned with the half-vector.
 */
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

/**
 * @brief Approximates the geometry attenuation using the Schlick-GGX model for a single direction.
 * 
 * Visual Effect: A helper function for 'GeometrySmith'. It calculates how much light is blocked 
 * by the microscopic bumps of a rough surface from a specific angle.
 * 
 * @param NdotV The dot product between the surface normal and the view (or light) direction.
 * @param roughness The roughness of the material.
 * 
 * @return A shadowing multiplier between 0.0 (fully shadowed) and 1.0 (unshadowed).
 */
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

/**
 * @brief Computes the Geometry function using Smith's method to resolve microfacet self-shadowing.
 * 
 * Visual Effect: Darkens rough surfaces at grazing angles. It ensures that microscopic peaks on 
 * the surface mask and shadow each other properly, preserving the physical conservation of energy.
 * 
 * @param N The normalized surface normal vector.
 * @param V The normalized view direction vector (towards the camera).
 * @param L The normalized light direction vector (towards the light source).
 * @param roughness The roughness of the material.
 * 
 * @return The combined geometry attenuation factor for both the view and light directions.
 */
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

/**
 * @brief Calculates the incoming light radiance from a Point Light.
 *
 * Visual Effect: Evaluates the light intensity based on the inverse-square law. The light gets 
 * exponentially dimmer the further the fragment is from the light source.
 * 
 * @param light The PointLight structure containing position, color, and intensity data.
 * 
 * @return The incoming radiance (color * intensity * distance attenuation) at the current 
 * fragment.
 */
vec3 PointRadiance(PointLight light)
{
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (distance * distance);
    return light.color * attenuation * light.intensity; // Radiance
}

/**
 * @brief Retrieves the constant light radiance from a Directional Light.
 * 
 * Visual Effect: Provides a steady, unattenuated wash of colored light over the fragment, 
 * independent of the fragment's position in space.
 * 
 * @param light The DirLight structure containing direction, color, and intensity data.
 * 
 * @return The constant incoming radiance (color * intensity).
 */
vec3 DirRadiance(DirLight light)
{
    return light.color * light.intensity; // Radiance
}

/**
 * @brief Calculates the incoming light radiance from a Spotlight, handling both distance and cone 
 * attenuation.
 * 
 * Visual Effect: Creates a beam of light. The fragment is brightly lit if it lies within the 
 * inner cone, receives fading light in the outer cone, and remains unlit outside the spotlight's 
 * radius.
 * 
 * @param light The SpotLight structure containing position, direction, and cone angle data.
 * @param L The normalized direction vector pointing from the fragment to the light source.
 * 
 * @return The incoming radiance modulated by both the inverse-square distance and the spotlight 
 * cone angles.
 */
vec3 SpotRadiance(SpotLight light, vec3 L)
{
    float d = length(light.position - FragPos);
    float cosTheta = dot(normalize(light.direction), -L);

    float angleAtt = clamp((cosTheta - light.outerCos) / (light.innerCos - light.outerCos),
        0.0, 1.0);
    float distance = length(light.position - FragPos);
    float distAtt = 1.0 / (distance * distance);
    return light.color * light.intensity * angleAtt * distAtt; // Radiance
}

/**
 * @brief Applies the ACES (Academy Color Encoding System) filmic tonemapping curve.

 * Visual Effect: Acts like a virtual camera lens, smoothly compressing high dynamic range (HDR) 
 * values into standard display colors (LDR). It prevents intensely bright lights from harshly 
 * clipping to pure white.
 * 
 * @param x The linear High Dynamic Range (HDR) color value.
 * 
 * @return The tonemapped Low Dynamic Range (LDR) color, clamped between 0.0 and 1.0.
 */
vec3 ACES(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

/**
 * @brief Performs Parallax Occlusion Mapping to displace texture coordinates based on a heightmap.
 * 
 * Visual Effect: Creates an optical illusion of deep 3D geometry on a flat polygon by shifting 
 * the texture UVs depending on the viewing angle and depth map.
 * 
 * @param texCoords The original, unaltered UV coordinates of the fragment.
 * @param viewDir The view direction vector transformed into Tangent Space.
 * 
 * @return The newly displaced UV coordinates to be used for sampling all other material textures.
 */
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 P = (viewDir.xy / max(viewDir.z, 0.001)) * material.height;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = 1.0 - texture(material.heightMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - texture(material.heightMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(material.heightMap, prevTexCoords).r)
                            - currentLayerDepth + layerDepth;
    
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

/**
 * @brief Determines if the current fragment is in a shadow using Percentage-Closer Filtering (PCF).
 *
 * Visual Effect: Casts shadows from the directional light onto the geometry. The PCF loops through 
 * neighboring pixels to slightly blur the shadow edges, producing softer, more realistic shadows.
 * 
 * @param fragPosLightSpace The fragment's world position transformed into the light's clipping space.
 * @param normal The normalized surface normal vector (used for bias calculation to prevent acne).
 * @param lightDir The normalized direction vector pointing towards the light source.
 * 
 * @return A shadow factor where 0.0 means completely lit and 1.0 means completely in shadow.
 */
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Normalisation [-1; 1] ==> [0; 1]

    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Shadow acne
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // With PCF (Percentage-Closer Filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

/**
 * @brief Evaluates the Cook-Torrance BRDF for a single light source.
 * 
 * Visual Effect: This is the core of the PBR rendering. It calculates exactly how a specific 
 * light source reacts with the fragment's material, balancing the diffuse color (albedo) with the 
 * metallic and specular reflections.
 * 
 * @param radiance The incoming light energy computed from the light source.
 * @param L The normalized direction vector to the light source.
 * @param V The normalized view direction vector.
 * @param N The normalized surface normal vector.
 * @param albedo The base color of the material.
 * @param roughness The surface roughness.
 * @param metallic The surface metallic factor (0.0 = dielectric, 1.0 = metal).
 * @param F0 The base reflectivity at normal incidence.
 * 
 * @return The outgoing radiance (Lo) reflected towards the camera for this specific light.
 */
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

/**
 * @brief Calculates the ambient lighting contribution using Image-Based Lighting (IBL).
 * 
 * Visual Effect: Fills in the shadows and unlit areas with global illumination from the Skybox. 
 * It gives metallic objects their mirror-like environmental reflections and gives matte objects 
 * a baseline realistic color tone derived from the surrounding atmosphere.
 * 
 * @param N The normalized surface normal vector.
 * @param V The normalized view direction vector.
 * @param F0 The base reflectivity at normal incidence.
 * @param albedo The base color of the material.
 * @param roughness The surface roughness.
 * @param metallic The surface metallic factor.
 * @param ao The ambient occlusion factor (0.0 = fully occluded, 1.0 = fully lit).
 *
 * @return The final ambient environmental lighting contribution (diffuse + specular).
 */
vec3 AmbientPBR(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic, float ao)
{
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = vec3(0.03);
    if(skybox.hasSkybox) irradiance = texture(skybox.irradianceMap, N).rgb;
    else irradiance = skybox.background * skybox.exposure;
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-V, N);
    
    const float MAX_REFLECTION_LOD = 10.0;
    vec3 prefilteredColor = vec3(0.03);

    if(skybox.hasSkybox) {
        prefilteredColor = textureLod(skybox.environmentMap, R, roughness * MAX_REFLECTION_LOD).rgb
                            * skybox.exposure;
    } else {
        prefilteredColor = skybox.background * skybox.exposure; 
    }

    vec2 envBRDF  = texture(skybox.brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao;
    return ambient;
}

/**
 * @brief Assembles all visual components: material sampling, lighting loops, shadows, and 
 * tonemapping.
 * 
 * Visual Effect: The final master compositor. It gathers the texture maps, computes the parallax, 
 * loops through all dynamic lights to accumulate PBR radiance, adds the ambient IBL, applies 
 * shadows, and outputs the final photorealistic pixel color.
 *
 * @return None. (Writes directly to the global 'FragColor' output variable).
 */
void FullRendering()
{
    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    vec3 V = normalize(viewPos - FragPos);

    // Parallax mapping
    vec2 texCoords = UV;
    if(material.hasHeightMap) {
        vec3 tangentViewDir = normalize(TangentViewPos - TangentFragPos);
        texCoords = ParallaxMapping(UV, tangentViewDir);
    }

    // Material selection
    if(material.hasNormalMap) {
        vec3 B = cross(T, N); // Bitangent
        mat3 TBN = mat3(T, B, N);
        vec3 normal = texture(material.normalMap, texCoords).rgb;
        normal = normal * 2.0 - 1.0;
        N = normalize(TBN * normal);
    }
    
    vec3 albedo = material.albedo;
    if (material.hasAlbedoMap && !PBR_ONLY)
        albedo = albedo * pow(texture(material.albedoMap, texCoords).rgb, vec3(2.2));

    // Gestion de la norme ORM (Occlusion - Roughness - Metallic) ---------------------------------
    float ao = material.ao;
    if (material.hasAOMap && !PBR_ONLY) ao = ao * texture(material.aoMap, texCoords).r;

    float roughness = material.roughness;
    if (material.hasRoughnessMap && !PBR_ONLY)
        roughness = roughness * texture(material.roughnessMap, texCoords).g;

    float metallic = material.metallic;
    if (material.hasMetallicMap && !PBR_ONLY)
        metallic = metallic * texture(material.metallicMap, texCoords).b;
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
#ifdef DIR_LIGHTS // Including Shadow map for DirLight[0]
    for(int i = 0; i < NB_DIR_LIGHTS; ++i) {
        vec3 L = normalize(-dirLights[i].direction); 
        vec3 radiance = DirRadiance(dirLights[i]);

        // Shadow mapping
        if(i == 0) {
            float shadow = ShadowCalculation(FragPosLightSpace, N, L);
            Lo += (1.0 - shadow) * PBR(radiance, L, V, N, albedo, roughness, metallic, F0);
        } else {
            Lo += PBR(radiance, L, V, N, albedo, roughness, metallic, F0);
        }
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
	
    if(toneMappingOn)
        color = ACES(color); // Academy Color Encoding System
    
    if(gammaCorrectionOn)
        color = pow(color, vec3(1.0/2.2));   // Gama correction
   
    FragColor = vec4(color, 1.0);
}



/*************************************************************************************************
 *                                         MAIN SECTION                                          *
 *************************************************************************************************/

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
        if (material.hasAlbedoMap && !PBR_ONLY)
            albedo = albedo * pow(texture(material.albedoMap, UV).rgb, vec3(2.2));
        FragColor = vec4(albedo, 1.0);
        break;

    case M_ROUGHNESS:
        float roughness = material.roughness;
        if (material.hasRoughnessMap && !PBR_ONLY)
            roughness = roughness * texture(material.roughnessMap, UV).g;
        FragColor = vec4(vec3(roughness), 1.0);
        break;

    case M_METALLIC:
        float metallic = material.metallic;
        if (material.hasMetallicMap && !PBR_ONLY)
            metallic = metallic * texture(material.metallicMap, UV).b;
        FragColor = vec4(vec3(metallic), 1.0);
        break;

    case M_AO:
        float ao = material.ao;
        if (material.hasAOMap && !PBR_ONLY) ao = ao * texture(material.aoMap, UV).r;
        FragColor = vec4(vec3(ao), 1.0);
        break;
    
    case M_HEIGHT:
        float height = material.height;
        if (material.hasHeightMap && !PBR_ONLY) height = texture(material.heightMap, UV).r;
        FragColor = vec4(vec3(height), 1.0);
        break;

    case M_SHADOW:
        float shadow = 0.0;
        #ifdef DIR_LIGHTS
            vec3 N = normalize(Normal);
            vec3 L = normalize(dirLights[0].direction); 
            shadow = ShadowCalculation(FragPosLightSpace, N, L);
        #endif
        FragColor = vec4(vec3(1.0 - shadow), 1.0);
        break;
    }
}