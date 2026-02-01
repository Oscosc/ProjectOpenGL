#include <ProjectIGAI/graphics/Object.hpp>

#include <ProjectIGAI/core/Logger.hpp>

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
    m_texture = TextureManager::getInstance().getTexture(name);
    if(m_texture)
        m_hasTexture = true;
}

void Object::bindTexture(Shader* shader) const
{
    if(!m_hasTexture) return;

    shader->setInt("objectTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Object::initGLObject()
{
    glGenVertexArrays(1, &this->m_VAO);
    glGenBuffers(1, &this->m_VBO);
    glGenBuffers(1, &this->m_EBO);

    glBindVertexArray(this->m_VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        this->m_indexes.size() * sizeof(unsigned int),
        this->m_indexes.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        this->m_vertices.size() * sizeof(Vertex),
        this->m_vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Object::updateMaterial(Shader *shader) const
{
    shader->use();
    
    shader->setVec3("material.color", this->getMaterial().matShader.color);
    shader->setFloat("material.roughness", this->getMaterial().matShader.roughness);
    shader->setFloat("material.metallic", this->getMaterial().matShader.metallic);
}
