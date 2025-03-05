#include "Ray.hpp"

Ray::Ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction)
{
    updateVertices(getVertices());
}

void Ray::draw(Shader shader)
{
    glBindVertexArray(VAO);

    shader.setVec3("color", 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINE_STRIP, 0, 2);

    // Optionnel : "déconnecte" le VAO
    glBindVertexArray(0);
}

ptsTab Ray::getVertices()
{
    return {m_origin, m_origin + m_direction * RAY_LENGTH};
}
