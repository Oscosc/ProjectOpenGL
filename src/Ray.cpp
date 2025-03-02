#include "Ray.hpp"

Ray::Ray(glm::vec3 start, glm::vec3 end)
{
    m_points.push_back(start);
    m_points.push_back(end);
    updateVertices(m_points);
}

void Ray::draw(Shader shader)
{
    glBindVertexArray(VAO);

    shader.setVec3("color", 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINE_STRIP, 0, m_points.size());

    // Optionnel : "déconnecte" le VAO
    glBindVertexArray(0);
}