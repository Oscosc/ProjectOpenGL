#pragma once

/**
 * @file BezierSurface.hpp
 * @brief Définition de la classe BezierSurface.
 * 
 * Ce fichier contient la représentation des surfaces de Béziers et leur implémentation dans le
 * cadre d'un programme OpenGL.
 * 
 * @author Oscar G.
 * @date 2025-03-01
 */

#include <ProjectIGAI/geometry/Surface.hpp>
#include <ProjectIGAI/core/utils.hpp>

/**
 * @class BezierSurface
 * @brief Objet représentant une surface de Bézier et son polygone de contrôle.
 * 
 * Cette classe gère les points de contrôle et une méthode de discrétisation de la courbe.
 * Cette classe hérite de la classe Object, ce qui fait qu'elle est affichable via la fonction
 * draw().
 */
class BezierSurface : public Surface
{
public:

    /**
     * @brief Construct a new Bezier Surface object using it's control points and various
     * informations such as transform and material.
     * 
     * @param controlPoints control points of the surface
     * @param transform transform of the surface
     * @param material material of the surface
     */
    BezierSurface(vec3Grid controlPoints,
        Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        Material material = DEFAULT_OBJECT_MATERIAL
    );

    /**
     * @brief Return the value (position) of a point on the surface base on uv.
     * 
     * @param uv vector between [0;0] and [1;1]
     * @return value of the surface at this index
     */
    glm::vec3 getValue(glm::vec2 uv) const override;

    /**
     * @brief Draw the surface on the screen, this function must be called in a
     * window drawing loop.
     * 
     * @param scene scene used for drawing (for shaders informations)
     */
    void draw(Scene* scene) const override;

protected:

    /**
     * @brief Compute all indexes of the surface based on it's discretization.
     * 
     */
    void computeIndexes();

    /**
     * @brief Compute normals of each point discretized on the surface.
     * 
     */
    void computeNormals();

    /**
     * @brief Compute UVs of each point discretized on the surface.
     * 
     */
    void computeUVs();
};
