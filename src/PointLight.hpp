#pragma once

#include "Light.hpp"

class PointLight : public Light
{
public:
    PointLight(glm::vec3 position = {0.f, 0.f, 0.f},
        glm::vec3 color = {1.f, 1.f, 1.f},
        float strength = 1.0)
        : Light(color, strength), m_position(position) {}

    glm::vec3 getPosition() { return m_position; }
    void setPosition(glm::vec3 position) { m_position = position; }

private:
    glm::vec3 m_position;
};