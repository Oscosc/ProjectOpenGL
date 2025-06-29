#pragma once

#include <glm/glm.hpp>

class Light
{
public:
    Light(glm::vec3 color, float strength) : m_color(color), m_strength(strength) {}
    virtual ~Light() = 0;

    glm::vec3 getColor() { return m_color; }
    void setColor(glm::vec3 color) { m_color = color; }
    
    float getStrength() { return m_strength; }
    void setStrength(float strength) { m_strength = strength; }

private:
    glm::vec3 m_color;
    float m_strength;
};

inline Light::~Light() {}