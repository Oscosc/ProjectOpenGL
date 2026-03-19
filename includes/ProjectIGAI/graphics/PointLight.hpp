#pragma once

#include <ProjectIGAI/graphics/Light.hpp>

#define DEFAULT_RADIUS 1.0f

/**
 * @brief This class represents a point light object, which is a positionned light that's diffuse
 * light all around it in a certain intensity. This light, do not have an associated visible
 * object, only visible effect on other objects.
 */
class PointLight : public Light
{
public:

    /**
     * @brief Construct a point light object.
     * 
     * @param position position of the light
     * @param radius radius of the light
     * @param color color of the light
     * @param strength intensity of the light
     */
    PointLight(Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        LightProperties material = DEFAULT_LIGHT_PROPERTIES,
        float radius = DEFAULT_RADIUS)
    : Light(transform, name, material), m_radius(radius) {}
    
    /**
     * @brief Get the Radius object
     * 
     * @return const float 
     */
    const float getRadius() const { return m_radius; }

    /**
     * @brief Set the Radius object
     * 
     * @param radius 
     */
    void setRadius(const float radius) { m_radius = radius; }

private:
    
    /** Radius of light emission */
    float m_radius;
};