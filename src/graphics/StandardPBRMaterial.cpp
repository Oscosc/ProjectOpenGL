#include <ProjectIGAI/graphics/StandardPBRMaterial.hpp>

#include <ProjectIGAI/graphics/TextureManager.hpp>
#include <ProjectIGAI/graphics/CubemapManager.hpp>
#include <ProjectIGAI/core/Scene.hpp>

void StandardPBRMaterial::bind(Scene* scene)
{
    // Activation shader
    m_shader->use();

    // Light updating
    scene->updateLigth(m_shader);

    // Ecriture des éléments principaux PBR
    m_shader->setVec3("material.albedo", albedo);
    m_shader->setFloat("material.roughness", roughness);
    m_shader->setFloat("material.metallic", metallic);
    m_shader->setFloat("material.ao", 1.0f);

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
        glBindTexture(GL_TEXTURE_2D, roughnessMap);
        m_shader->setInt("material.roughnessMap", 1);
        m_shader->setBool("material.hasRoughnessMap", true);
    } else {
        m_shader->setBool("material.hasRoughnessMap", false);
    }

    // Ecriture éventuelle de la metallicMap si existante
    if(metallicMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallicMap);
        m_shader->setInt("material.metallicMap", 2);
        m_shader->setBool("material.hasMetallicMap", true);
    } else {
        m_shader->setBool("material.hasMetallicMap", false);
    }

    // Ecriture éventuelle de la metallicMap si existante
    if(aoMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, aoMap);
        m_shader->setInt("material.aoMap", 2);
        m_shader->setBool("material.hasAOMap", true);
    } else {
        m_shader->setBool("material.hasAOMap", false);
    }

    // Ecriture de la normalMap si existante
    if(normalMap) {
        glad_glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        m_shader->setInt("material.normalMap", 3);
        m_shader->setBool("material.hasNormalMap", true);
    } else {
        m_shader->setBool("material.hasNormalMap", false);
    }

    // Binding de la skybox
    m_shader->setInt("skybox.irradianceMap", 10);
    m_shader->setInt("skybox.environmentMap", 11);
    m_shader->setInt("skybox.brdfLUT", 12);

    if(scene->skyboxActive()) {
        Cubemap* skyboxID = CubemapManager::getInstance().getResource(scene->skyboxName());
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID->irradiance);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID->environment);

        glActiveTexture(GL_TEXTURE12);
        GLuint brdfLUT_ID = TextureManager::getInstance().loadTexture("resources/textures/ibl_brdf_lut.png"); 
        glBindTexture(GL_TEXTURE_2D, brdfLUT_ID);

        m_shader->setBool("skybox.hasSkybox", true);
    } else {
        m_shader->setVec3("skybox.background", scene->getBackgroundColor());
        m_shader->setBool("skybox.hasSkybox", false);
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

void StandardPBRMaterial::setNormalTexture(const std::string &path)
{
    this->normalMap = TextureManager::getInstance().loadTexture(path);
}

void StandardPBRMaterial::setAOTexture(const std::string &path)
{
    this->aoMap = TextureManager::getInstance().loadTexture(path);
}
