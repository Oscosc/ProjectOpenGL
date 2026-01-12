#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include <extern/camera.hpp>

/**
 * @brief Class representing a ray casted from camera. Can be used to simulate ray-tracing in
 * raster mode, but ray-tracing window are drown directly from shaders and do not use this class.
 * 
 * @warning DEPRECATED, UNUSED
 */
class Ray
{
public:

    /**
     * @brief Construct a new empty ray object.
     */
    Ray() {}

    /**
     * @brief Construct a new Ray object with it's origin and it's direction.
     * 
     * @param origin origin of the ray
     * @param direction normalized direction of the ray
     */
    Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin), m_direction(direction) {}

    /**
     * @brief Construct a new Ray object based on camera and screen space informations.
     * 
     * @param x screen x position
     * @param y screen y position
     * @param width screen width
     * @param height screen height
     * @param radianFOV FOV of the camera
     * @param cameraToWorld camera to world matrix
     * @param origin origin of the ray (camera position)
     */
    Ray(const float& x, const float& y, const float& width, const float& height,
        const float& radianFOV, const glm::mat4& cameraToWorld, const glm::vec3& origin);
    
    /**
     * @brief return the origin of the ray.
     */
    const glm::vec3& origin() const { return m_origin; }

    /**
     * @brief return the normalized direction of the ray.
     */
    const glm::vec3& direction() const { return m_direction; }
    
    /**
     * @brief Compute the position of the ray at a time t.
     * 
     * @param t "time" value (supposing that's a ray velocity is equal to 1 in it's direction)
     * @return position of the ray at time t
     */
    glm::vec3 at(float t) const { return m_origin + t * m_direction; }

private:

    glm::vec3 m_origin;
    glm::vec3 m_direction;
};