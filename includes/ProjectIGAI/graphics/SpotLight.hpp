#pragma once

#include <ProjectIGAI/graphics/Light.hpp>

/**
 * @brief Class representing a Spot Light object.
 * 
 */
class SpotLight : public Light
{
public:

    /**
     * @brief Construct a new Spot Light object
     * 
     * @param direction direction of the spot
     * @param position position of the spot
     * @param cutOff cut-off angle
     * @param outerCutOff outer cut-off angle
     * @param material material of the spotlight (emmited color)
     */
    SpotLight(glm::vec3 direction = {0.f, -1.f, 0.f},
        glm::vec3 position = {0.f, 0.f, 0.f},
        float cutOff = 0.0,
        float outerCutOff = 1.0f,
        LightMaterial material = DEFAULT_LIGHT_MATERIAL)
    : Light(material), m_direction(direction), m_position(position),
    m_cutOff(cutOff), m_outerCutOff(outerCutOff) {}
    
    /**
     * @brief Return the direction of the spot
     * 
     * @return spot's direction
     */
    glm::vec3 getDirection() const { return m_direction; }

    /**
     * @brief Return the position of the spot
     * 
     * @return spot's position
     */
    glm::vec3 getPosition() const { return m_position; }

    /**
     * @brief Return the cut-off of the spot
     * 
     * @return spot's cut-off
     */
    float getCutOff() const { return m_cutOff; }

    /**
     * @brief Return the outer cut-off of the spot
     * 
     * @return spot's outer cut-off
     */
    float getOuterCutOff() const { return m_outerCutOff; }
    
    /**
     * @brief Set the spot direction
     * 
     * @param direction new spot's direction
     */
    void setDirection(glm::vec3 direction) { m_direction = direction; }

    /**
     * @brief Set the spot position
     * 
     * @param direction new spot's position
     */
    void setPosition(glm::vec3 position) { m_position = position; }

    /**
     * @brief Set the spot cut-off
     * 
     * @param direction new spot's cut-off
     */
    void setCutOff(float cutOff) { m_cutOff = cutOff; }

    /**
     * @brief Set the spot outer cut-off
     * 
     * @param direction new spot's outer cut-off
     */
    void setOuterCutOff(float outerCutOff) { m_outerCutOff = outerCutOff; }
    
private:

    /** attributes of the spot light */
    glm::vec3 m_direction;
    glm::vec3 m_position;
    float m_cutOff;
    float m_outerCutOff;
};