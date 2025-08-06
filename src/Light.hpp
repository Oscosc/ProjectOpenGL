#pragma once

#include <glm/glm.hpp>

#define DEFAULT_LIGHT_MATERIAL {glm::vec3(0.2), glm::vec3(0.5), glm::vec3(1.0)}

struct LightMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

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
    Light(LightMaterial material = DEFAULT_LIGHT_MATERIAL) : m_material(material) {}

    /**
     * @brief default destructor (virtual to keep class abstract).
     */
    virtual ~Light() = 0;

    LightMaterial getLightMaterial() const { return m_material; }
    void setLightMaterial(LightMaterial material) { m_material = material; }

private:
    LightMaterial m_material;
};

/**
 * @brief Virtual destructor declaration for compilation purposes.
 */
inline Light::~Light() {}