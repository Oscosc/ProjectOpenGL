#ifndef RAY_HPP
#define RAY_HPP

#include "Object.hpp"

#define RAY_LENGTH 100.0f


class Ray : public Object
{
public:
    Ray(glm::vec3 origin, glm::vec3 direction);
    void draw(Shader shader) override;

private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;

    ptsTab getVertices();
};

#endif // RAY_HPP