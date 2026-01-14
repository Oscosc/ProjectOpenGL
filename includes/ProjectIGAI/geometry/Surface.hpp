#pragma once

#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/core/utils.hpp>

/**
 * @brief Class representing a general surface, can be a Surface using B-Splines or Bezier
 * equations.
 * 
 */
class Surface : public Object
{
public:

    /**
     * @brief Construct a new Surface
     * 
     * @param transform transformation of the surface
     * @param material material of the surface
     */
    Surface(Transform transform, Material material) : Object(transform, material) {}

    /**
     * @brief Get the 3D point position of a uv value in the surface
     * 
     * @param uv normal coordinates on the surface
     * @return associated 3D postion
     */
    virtual glm::vec3 getValue(glm::vec2 uv) const = 0;

protected:

    /**
     * @brief Draw the control polygon of the surface on screen.
     * 
     * @warning DEPRECATED
     */
    void drawControlPolygon() const;

    /**
     * @brief Init the control polygon buffers.
     * 
     * @warning DEPRECATED
     * 
     */
    void initGLControlPolygon();

    /** VAO and VBO for control polygon */
    unsigned int m_controlVAO;
    unsigned int m_controlVBO;

    /** Control polygon and parameters */
    vec3Grid m_controlPoints;
    unsigned int m_sizeU;
    unsigned int m_sizeV;
};