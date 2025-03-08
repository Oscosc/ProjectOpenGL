#ifndef RAY_HPP
#define RAY_HPP

#include "Object.hpp"
#include "Sphere.hpp"

#include "AppContext.hpp"

#define RAY_LENGTH 100.0f


class Ray : public Object
{
public:
    Ray(glm::vec3 origin, glm::vec3 direction);
    void draw(Shader shader) override;

    bool intersect(const Sphere &sphere, float &point, glm::vec3 &new_direction) const;
    glm::vec3 getPoint(float t) const;

private:
    glm::vec3 m_direction;

    glm::vec3 getDirection() const;

    ptsTab getVertices();
};

#endif // RAY_HPP