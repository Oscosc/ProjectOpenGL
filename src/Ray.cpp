#include "Ray.hpp"


Ray::Ray(glm::vec3 origin, glm::vec3 direction) : m_direction(direction)
{
    setOrigin(origin);
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


bool Ray::intersect(const Sphere &sphere, float &point, glm::vec3 &new_direction) const
{
    // CALCUL DU POINT D'INTERSECTION
    float t0, t1; // Solutions for t if the ray intersects the sphere
    glm::vec3 L = this->getOrigin() - sphere.getOrigin();
    float a = glm::dot(this->getDirection(), this->getDirection());
    float b = 2 * glm::dot(this->getDirection(), L);
    float c = glm::dot(L, L) - sphere.getRadius() * sphere.getRadius();
    if (!solveQuadratic(a, b, c, t0, t1)) return false;
    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1; // If t0 is negative, let's use t1 instead.
        if (t0 < 0) return false; // Both t0 and t1 are negative.
    }

    point = t0;

    // CALCUL DE LA NOUVELLE DIRECTION
    glm::vec3 norm = glm::normalize(getPoint(point) - sphere.getOrigin());
    glm::vec3 dir = getDirection();
    new_direction = dir - 2 * glm::dot(dir, norm) * norm;

    return true;
}


glm::vec3 Ray::getPoint(float t) const
{
    return getOrigin() + getDirection() * t;
}


glm::vec3 Ray::getDirection() const
{
    return m_direction;
}


ptsTab Ray::getVertices()
{
    return {m_origin, m_origin + m_direction * RAY_LENGTH};
}
