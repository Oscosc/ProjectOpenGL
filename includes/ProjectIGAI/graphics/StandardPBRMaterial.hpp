#pragma once

#include <ProjectIGAI/graphics/Material.hpp>

class StandardPBRMaterial : public Material
{
public:

    StandardPBRMaterial() : Material(ShaderManager::getInstance().getResource("lighted")) {}

    // Material values
    glm::vec3 albedo = glm::vec3(1.0);
    float roughness = 0.5f;
    float metallic = 0.0f;

    // Alternative : material maps
    GLuint albedoMap    = 0;
    GLuint roughnessMap = 0;
    GLuint metallicMap  = 0;

    void bind(Scene* scene) override;

    void setAlbedoTexture(const std::string& path);

    void setRoughnessTexture(const std::string& path);

    void setMetallicTexture(const std::string& path);
};