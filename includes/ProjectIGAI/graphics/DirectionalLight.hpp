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
    DirectionalLight(Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        LightProperties material = DEFAULT_LIGHT_PROPERTIES)
    : Light(transform, name, material) {}
    
private:
};