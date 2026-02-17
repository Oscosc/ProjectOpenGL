#include <ProjectIGAI/graphics/StandardPBRMaterial.hpp>

#include <ProjectIGAI/graphics/TextureManager.hpp>

void StandardPBRMaterial::bind(Scene* scene)
{
    // Activation shader
    m_shader->use();

    // Ecriture des éléments principaux PBR
    m_shader->setVec3("material.albedo", albedo);
    m_shader->setFloat("material.roughness", roughness);
    m_shader->setFloat("material.metallic", metallic);

    // Ecriture éventuelle de l'albedoMap si existante
    if(albedoMap) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        m_shader->setInt("material.albedoMap", 0);
        m_shader->setBool("material.hasAlbedoMap", true);
    } else {
        m_shader->setBool("material.hasAlbedoMap", false);
    }

    // Ecriture éventuelle de la roughnessMap si existante
    if(roughnessMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        m_shader->setInt("material.roughnessMap", 1);
        m_shader->setBool("material.hasRoughnessMap", true);
    } else {
        m_shader->setBool("material.hasRoughnessMap", false);
    }

    // Ecriture éventuelle de la metallicMap si existante
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
