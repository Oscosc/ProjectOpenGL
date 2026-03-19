#pragma once

#include <ProjectIGAI/graphics/Light.hpp>

/**
 * @brief Class representing a Spot Light object.
 * 
 */
class SpotLight : public PointLight
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
    SpotLight(Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        LightProperties material = DEFAULT_LIGHT_PROPERTIES,
        float radius = DEFAULT_RADIUS,
        float cutOff = 0.0,
        float outerCutOff = 1.0f)
    : PointLight(transform, name, material, radius),
    m_cutOff(cutOff), m_outerCutOff(outerCutOff) {}

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
    float m_cutOff;
    float m_outerCutOff;
};