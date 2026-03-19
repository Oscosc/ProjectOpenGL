#pragma once

#include <glm/glm.hpp>
#include <ProjectIGAI/graphics/Node.hpp>

#define DEFAULT_LIGHT_PROPERTIES {glm::vec3(1.0), 1.0f}

struct LightProperties {
    glm::vec3 color;
    float intensity;
};

/**
 * @brief Abstract class for lights objects.
 */
class Light : public Node
{
public:

    /**
     * @brief Default constructor of a light object called by sub-classes.
     * 
     * @param color Color emmited by this light
     * @param strength Intensity of the light emmited
     */
    Light(Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        LightProperties material = DEFAULT_LIGHT_PROPERTIES)
        : Node(transform, name), m_material(material) {}

    /**
     * @brief default destructor (virtual to keep class abstract).
     */
    virtual ~Light() = 0;

    /**
     * @brief Get light properties.
     */
    const LightProperties& getLightMaterial() const { return m_material; }

    /**
     * @brief Set light properties.
     * 
     * @param material new properties
     */
    void setLightMaterial(LightProperties material) { m_material = material; }
    
    /**
     * @brief Set light color.
     * 
     * @param color new color
     */
    void setColor(glm::vec3 color) { m_material.color = color; }

    /**
     * @brief Set light intensity.
     * 
     * @param intensity new intensity
     */
    void setIntensity(float intensity) { m_material.intensity = intensity; }

private:

    /** Properties of the light */
    LightProperties m_material;
};

/**
 * @brief Virtual destructor declaration for compilation purposes.
 */
inline Light::~Light() {}