#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include <extern/camera.hpp>

class Ray
{
public:
    Ray() {}
    Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin), m_direction(direction) {}
    Ray(const float& x, const float& y, const float& width, const float& height,
        const float& radianFOV, const glm::mat4& cameraToWorld, const glm::vec3& origin);

    const glm::vec3& origin() const { return m_origin; }
    const glm::vec3& direction() const { return m_direction; }

    glm::vec3 at(float t) const { return m_origin + t * m_direction; }

private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
};