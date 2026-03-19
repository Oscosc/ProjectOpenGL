#pragma once

#include <ProjectIGAI/graphics/Material.hpp>

class StandardPBRMaterial : public Material
{
public:

    StandardPBRMaterial();

    // Material values
    glm::vec3 albedo = glm::vec3(1.0);
    float roughness = 1.0f;
    float metallic = 1.0f;
    float ao = 1.0f;
    float height = 0.f;

    // Alternative : material maps
    GLuint albedoMap    = 0;
    GLuint roughnessMap = 0;
    GLuint metallicMap  = 0;
    GLuint normalMap    = 0;
    GLuint aoMap        = 0;
    GLuint heightMap    = 0;

    void bind(Scene* scene) override;

    void setAlbedoTexture(const std::string& path);
    void setRoughnessTexture(const std::string& path);
    void setMetallicTexture(const std::string& path);
    void setNormalTexture(const std::string& path);
    void setAOTexture(const std::string& path);
    void setHeightTexture(const std::string& path);

    void setAlbedoMapID(GLuint id) { this->albedoMap = id; }
    void setRoughnessMapID(GLuint id) { this->roughnessMap = id; }
    void setMetallicMapID(GLuint id) { this->metallicMap = id; }
    void setNormalMapID(GLuint id) { this->normalMap = id; }
    void setAOMapID(GLuint id) { this->aoMap = id; }
    void setHeightMapID(GLuint id) { this->heightMap = id; }

private:

    GLuint m_brdfLUT_ID;

};