#pragma once

#include <ProjectIGAI/graphics/Light.hpp>

/**
 * @brief Class representing a directional light object.
 * 
 */
class DirectionalLight : public Light
{
public:

    /**
     * @brief Construct a new Directional Light
     * 
     * @param direction direction of the light
     * @param material material of the light
     */
    DirectionalLight(glm::vec3 direction = {0.f, -1.f, 0.f},
    LightMaterial material = DEFAULT_LIGHT_MATERIAL)
    : Light(material), m_direction(direction) {}
    
    /**
     * @brief Get light direction
     * 
     * @return light direction
     */
    glm::vec3 getDirection() const { return m_direction; }
    
    /**
     * @brief Set a new light direction
     * 
     * @param direction of the light
     */
    void setDirection(glm::vec3 direction) { m_direction = direction; }
    
private:
    
    /** Direction of the light */
    glm::vec3 m_direction;
};