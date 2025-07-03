#pragma once

#include "Light.hpp"

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
     * @param color color of the light
     * @param strength intensity of the light
     */
    PointLight(glm::vec3 position = {0.f, 0.f, 0.f},
        glm::vec3 color = {1.f, 1.f, 1.f},
        float strength = 1.0)
        : Light(color, strength), m_position(position) {}

    /**
     * @brief return the light position.
     */
    glm::vec3 getPosition() { return m_position; }

    /**
     * @brief set the ligth to a new position.
     * 
     * @param position new position
     */
    void setPosition(glm::vec3 position) { m_position = position; }

private:
    glm::vec3 m_position;
};