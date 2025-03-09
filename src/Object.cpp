#include "Object.hpp"


Object::Object() : m_origin(glm::vec3(0.0f))
{
    // Création du VAO et du VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // "Connexion" au VAO et au VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Optionnel : on se "déconnecte" du VAO et du VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


Object::~Object()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}



void Object::updateVertices(ptsTab points)
{
    // "Connexion" au VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        points.size() * sizeof(glm::vec3),
        points.data(),
        GL_DYNAMIC_DRAW
    );

    // Lier les données aux attributs de vertex
    glEnableVertexAttribArray(0);

    // Optionnel : on se "déconnecte" du VAO et du VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::vec3 Object::getOrigin() const {return m_origin;}

void Object::setOrigin(glm::vec3 value) {m_origin = value;}