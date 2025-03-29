#include "Object.hpp"


Object::Object(bool enableNormal, bool enableUV) :
    m_origin(glm::vec3(0.0f)), m_color(glm::vec3(1.0f)), m_ambient(OBJECT_AMBIENT_STRENGTH)
{
    // Création du VAO et du VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // "Connexion" au VAO et au VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    unsigned int nbVec = (enableNormal) ? 2 : 1; // Si on a la normale alors on a 2 vec3
    nbVec += (enableUV) ? 1 : 0;                 // Si on a les UVs alors on a un vec3 de plus

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nbVec * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    if(enableNormal) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nbVec * sizeof(glm::vec3), (void*)(1 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);
    }

    if(enableUV) {
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, nbVec * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
    }

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

    // Optionnel : on se "déconnecte" du VAO et du VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::vec3 Object::getOrigin() const {return m_origin;}

void Object::setOrigin(glm::vec3 value) {m_origin = value;}

glm::vec3 Object::getColor() const {return m_color;}

void Object::setColor(glm::vec3 value) {m_color = value;}

float Object::getAmbient() const {return m_ambient;}

void Object::setAmbient(float value) {m_ambient = value;}

// const std::vector<Triangle>* Object::getTriangles() {return &m_triangles;}
