#include "Object.hpp"

void Object::debugMaterial() const
{
    std::cout << "  |- Ambient   : " << glm::to_string(getMaterial().matShader.ambient) << std::endl;
    std::cout << "  |- Diffuse   : " << glm::to_string(getMaterial().matShader.diffuse) << std::endl;
    std::cout << "  |- Specular  : " << glm::to_string(getMaterial().matShader.specular) << std::endl;
    std::cout << "  |- Shininess : " << getMaterial().matShader.shininess << std::endl;
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
    
    shader->setVec3("material.ambient", this->getMaterial().matShader.ambient);
    shader->setVec3("material.diffuse", this->getMaterial().matShader.diffuse);
    shader->setVec3("material.specular", this->getMaterial().matShader.specular);
    shader->setFloat("material.shininess", this->getMaterial().matShader.shininess);
}
