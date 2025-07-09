#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "../includes/camera.hpp"
#include "Sphere.hpp"

class Ray
{
public:
    Ray() {}
    Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin), m_direction(direction) {}
    Ray(const float& x, const float& y, const float& width, const float& height,
        const float& fov, const glm::mat4& view, const glm::vec3& origin);

    const glm::vec3& origin() { return m_origin; }
    const glm::vec3& direction() { return m_direction; }

    glm::vec3 at(float t) const { return m_origin + t * m_direction; }

    float hitSphere(Sphere& sphere);

private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
};