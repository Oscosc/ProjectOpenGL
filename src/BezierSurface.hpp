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

#include "Surface.hpp"
#include "utils.hpp"

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

    BezierSurface(vec3Grid controlPoints,
        Transform transform = DEFAULT_OBJECT_TRANSFORM,
        Material material = DEFAULT_OBJECT_MATERIAL
    );

    glm::vec3 getValue(glm::vec2 uv) const override;

    void draw(Scene* scene) const override;

protected:

    void computeIndexes();
    void computeNormals();
};
