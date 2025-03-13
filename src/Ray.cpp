#include "Ray.hpp"


Ray::Ray() : m_direction(glm::vec3(0.0f)), m_bounces(0)
{
    setOrigin(glm::vec3(0.0f));
}


Ray::Ray(glm::vec3 origin, glm::vec3 direction) : m_direction(direction), m_bounces(0)
{
    setOrigin(origin);

    updateVertices(getVertices());
}


Ray::Ray(glm::vec3 origin, glm::vec3 direction, ptsTab intersections, glm::vec3 reflexion) :
    m_direction(direction), m_bounces(intersections.size())
{
    setOrigin(origin);
    
    float remainingLength = RAY_LENGTH;
    glm::vec3 lastPosition = origin;
    ptsTab vertices = {origin};

    for(glm::vec3 point : intersections) {

        glm::vec3 nextDirection = point - lastPosition;
        float distanceToNext = glm::length(nextDirection);

        // S'il ne reste pas "assez" de rayon, on calcule le dernier point pour que la longueur
        // totale soit égale à RAY_LENGTH
        if (distanceToNext > remainingLength) {
            reflexion = glm::normalize(nextDirection);
            vertices.push_back(lastPosition + nextDirection * remainingLength);
            return;
        }

        // Sinon, on ajoute simplement le segment et on met à jour les variables
        remainingLength -= distanceToNext;
        lastPosition = point;
        vertices.push_back(point);
    }

    if (remainingLength > 0)
        vertices.push_back(lastPosition + reflexion * remainingLength);

    updateVertices(vertices);
}


void Ray::draw(Shader shader)
{
    glBindVertexArray(VAO);

    if(m_bounces == 0) {
        shader.setVec3("color", 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
    }
    else {
        shader.setVec3("color", 0.0f, 1.0f, 0.0f); 
        glDrawArrays(GL_LINE_STRIP, 0, 2);
        shader.setVec3("color", 1.0f, 0.0f, 0.0f); 
        glDrawArrays(GL_LINE_STRIP, 1, m_bounces + 1);
    }

    // Optionnel : "déconnecte" le VAO
    glBindVertexArray(0);
}


glm::vec3 Ray::getPoint(float t) const
{
    return getOrigin() + getDirection() * t;
}


glm::vec3 Ray::getDirection() const
{
    return m_direction;
}

void Ray::setDirection(glm::vec3 value)
{
    m_direction = value;
}

ptsTab Ray::getVertices()
{
    return {m_origin, m_origin + m_direction * RAY_LENGTH};
}
