#pragma once

#include <glm/glm.hpp>

/**
 * @brief Abstract class for lights objects.
 */
class Light
{
public:

    /**
     * @brief Default constructor of a light object called by sub-classes.
     * 
     * @param color Color emmited by this light
     * @param strength Intensity of the light emmited
     */
    Light(glm::vec3 color, float strength) : m_color(color), m_strength(strength) {}

    /**
     * @brief default destructor (virtual to keep class abstract).
     */
    virtual ~Light() = 0;

    /**
     * @brief Return the color of the light.
     */
    glm::vec3 getColor() { return m_color; }

    /**
     * @brief Set a new color for this light.
     * 
     * @param color new light color.
     */
    void setColor(glm::vec3 color) { m_color = color; }
    
    /**
     * @brief Return the intensity of the light.
     */
    float getStrength() { return m_strength; }

    /**
     * @brief Set a new intensity for this light.
     * 
     * @param strength new intensity.
     */
    void setStrength(float strength) { m_strength = strength; }

private:
    glm::vec3 m_color;
    float m_strength;
};

/**
 * @brief Virtual destructor declaration for compilation purposes.
 */
inline Light::~Light() {}