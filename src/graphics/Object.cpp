#include <ProjectIGAI/graphics/Object.hpp>

#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/CubemapManager.hpp>

void Object::debugMaterial() const
{
    std::cout << "  |- Color     : " << glm::to_string(getMaterial().matShader.color) << std::endl;
    std::cout << "  |- Roughness : " << getMaterial().matShader.roughness << std::endl;
    std::cout << "  |- Metallic  : " << getMaterial().matShader.metallic << std::endl;
}

glm::mat4 Object::getModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->m_transform.position);
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.y), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.z), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, this->m_transform.scale);

    return model;
}

void Object::addTexture(std::string name)
{
    m_texture = *TextureManager::getInstance().getResource(name);
    if(m_texture)
        m_hasTexture = true;
}

void Object::bindTexture(Shader* shader, Scene* scene) const
{
    shader->setInt("objectTexture", 0);
    glActiveTexture(GL_TEXTURE0);

    if(m_hasTexture)
        glBindTexture(GL_TEXTURE_2D, m_texture);
    
    else
        glBindTexture(GL_TEXTURE_2D, TextureManager::getInstance().getDefaultTexture());

    shader->setInt("skybox", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *CubemapManager::getInstance().getResource(scene->skyboxName()));
}

void Object::updateMaterial(Shader *shader) const
{
    shader->use();
    
    shader->setVec3("material.color", this->getMaterial().matShader.color);
    shader->setFloat("material.roughness", this->getMaterial().matShader.roughness);
    shader->setFloat("material.metallic", this->getMaterial().matShader.metallic);
}
