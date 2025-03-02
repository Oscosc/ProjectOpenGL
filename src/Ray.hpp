#ifndef RAY_HPP
#define RAY_HPP

#include "Object.hpp"

class Ray : public Object
{
public:
    Ray(glm::vec3 start, glm::vec3 end);
    void draw(Shader shader) override;

private:
    ptsTab m_points;
};

#endif // RAY_HPP