#ifndef RAY_HPP
#define RAY_HPP

#include "Object.hpp"
#include "Sphere.hpp"

#include "AppContext.hpp"

#define RAY_LENGTH 100.0f


class Ray : public Object
{
public:
    Ray();
    Ray(glm::vec3 origin, glm::vec3 direction);
    Ray(glm::vec3 origin, glm::vec3 direction, ptsTab intersections, glm::vec3 reflexion);

    void draw(Shader shader) override;

    glm::vec3 getPoint(float t) const;
    glm::vec3 getDirection() const;
    void setDirection(glm::vec3 value);

private:
    glm::vec3 m_direction;
    unsigned int m_bounces;

    ptsTab getVertices();
};

#endif // RAY_HPP