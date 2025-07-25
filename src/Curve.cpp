#include "Curve.hpp"

void Curve::drawControlPolygon() {
    glBindVertexArray(this->m_controlVAO);

    glPointSize(DEFAULT_CONTROL_SIZE);
    this->m_material.shader->use();
    this->m_material.shader->setVec3("material.ambient", DEFAULT_CONTROL_COLOR);

    switch(m_controlPolygonVisibility) {
        case OFF: break;
        case POINTS: glDrawArrays(GL_POINTS, 0, this->m_controlPoints.size()); break;
        case LINES: glDrawArrays(GL_LINE_STRIP, 0, this->m_controlPoints.size()); break;
    }
}

void Curve::initGLControlPolygon() {
    glGenVertexArrays(1, &this->m_controlVAO);
    glGenBuffers(1, &this->m_controlVBO);

    glBindVertexArray(this->m_controlVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_controlVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        this->m_vertices.size() * sizeof(Vertex),
        defaultPointToVertex(this->m_controlPoints).data(),
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

/**
 * @brief Renvoie la valeur du triangle de Pascal pour un i et un n donnés en paramètre.
 */
int PascalValue(int i, int n)
{
    if(i == 0 || i == n)
        return 1;
    else
        return PascalValue(i, n-1) + PascalValue(i-1, n-1);
}

/**
 * @brief Calcule la valeur du polynôme de Bernstein au point u en tenant compte de n et i.
 * 
 * https://fr.wikipedia.org/wiki/Polyn%C3%B4me_de_Bernstein 
 */
float bersteinValue(float u, int i, int n)
{
    int p = PascalValue(i, n);
    return p * pow(u, i) * pow((1.0f - u), n-i);
}

std::vector<Vertex> defaultPointToVertex(const vec3Array& points) {
    std::vector<Vertex> vertices;
    vertices.resize(points.size());

    unsigned int i = 0;
    for(glm::vec3 p : points) {
        vertices[i] = {p, glm::vec3(1.f), glm::vec3(0.f)};
        ++i;
    }

    return vertices;
}