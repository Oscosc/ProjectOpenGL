#pragma once

#include <ProjectIGAI/graphics/Material.hpp>

class StandardPBRMaterial : public Material
{
public:

    StandardPBRMaterial() : Material(ShaderManager::getInstance().getResource("lighted")) {}

    // Material values
    glm::vec3 albedo = glm::vec3(1.0);
    float roughness = 1.0f;
    float metallic = 1.0f;

    // Alternative : material maps
    GLuint albedoMap    = 0;
    GLuint roughnessMap = 0;
    GLuint metallicMap  = 0;
    GLuint aoMap        = 0;

    // Other maps
    GLuint normalMap = 0;

    void bind(Scene* scene) override;

    void setAlbedoTexture(const std::string& path);

    void setRoughnessTexture(const std::string& path);

    void setMetallicTexture(const std::string& path);

    void setNormalTexture(const std::string& path);

    void setAOTexture(const std::string& path);
};