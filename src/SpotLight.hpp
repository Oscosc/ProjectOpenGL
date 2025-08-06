#pragma once

#include "Light.hpp"

class SpotLight : public Light
{
public:

    SpotLight(glm::vec3 direction = {0.f, -1.f, 0.f},
        glm::vec3 position = {0.f, 0.f, 0.f},
        float cutOff = 0.0,
        float outerCutOff = 1.0f,
        LightMaterial material = DEFAULT_LIGHT_MATERIAL)
    : Light(material), m_direction(direction) {}

    glm::vec3 getDirection() const { return m_direction; }
    glm::vec3 getPosition() const { return m_position; }
    float getCutOff() const { return m_cutOff; }
    float getOuterCutOff() const { return m_outerCutOff; }

    void setDirection(glm::vec3 direction) { m_direction = direction; }
    void setPosition(glm::vec3 position) { m_position = position; }
    void setCutOff(float cutOff) { m_cutOff = cutOff; }
    void setOuterCutOff(float outerCutOff) { m_outerCutOff = outerCutOff; }
    
private:
    glm::vec3 m_direction;
    glm::vec3 m_position;
    float m_cutOff;
    float m_outerCutOff;
};