#pragma once

#include <glm/glm.hpp>

/**
 * @brief Structure that's contain information about a Camera projection/view matrix
 */
struct ProjViewMatrix {
    glm::mat4 projection;
    glm::mat4 view;
};