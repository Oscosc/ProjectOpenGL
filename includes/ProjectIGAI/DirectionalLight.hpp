#pragma once

#include <ProjectIGAI/Light.hpp>

class DirectionalLight : public Light
{
public:

    DirectionalLight(glm::vec3 direction = {0.f, -1.f, 0.f},
    LightMaterial material = DEFAULT_LIGHT_MATERIAL)
    : Light(material), m_direction(direction) {}

    glm::vec3 getDirection() const { return m_direction; }

    void setDirection(glm::vec3 direction) { m_direction = direction; }
    
private:
    glm::vec3 m_direction;
};