#include <ProjectIGAI/graphics/StandardPBRMaterial.hpp>

#include <ProjectIGAI/graphics/TextureManager.hpp>

void StandardPBRMaterial::bind(Scene* scene)
{
    m_shader->use();

    m_shader->setVec3("material.albedo", albedo);
    m_shader->setFloat("material.roughness", roughness);
    m_shader->setFloat("material.metallic", metallic);

    if(albedoMap) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        m_shader->setInt("material.albedoMap", 0);
        m_shader->setBool("material.hasAlbedoMap", true);
    } else {
        m_shader->setBool("material.hasAlbedoMap", false);
    }

    if(roughnessMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        m_shader->setInt("material.roughnessMap", 1);
        m_shader->setBool("material.hasRoughnessMap", true);
    } else {
        m_shader->setBool("material.hasRoughnessMap", false);
    }

    if(metallicMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        m_shader->setInt("material.metallicMap", 2);
        m_shader->setBool("material.hasMetallicMap", true);
    } else {
        m_shader->setBool("material.hasMetallicMap", false);
    }
}

void StandardPBRMaterial::setAlbedoTexture(const std::string &path)
{
    this->albedoMap = TextureManager::getInstance().loadTexture(path);
}

void StandardPBRMaterial::setRoughnessTexture(const std::string &path)
{
    this->roughnessMap = TextureManager::getInstance().loadTexture(path);
}

void StandardPBRMaterial::setMetallicTexture(const std::string &path)
{
    this->metallicMap = TextureManager::getInstance().loadTexture(path);
}
