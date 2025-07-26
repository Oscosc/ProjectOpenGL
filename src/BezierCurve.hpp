#ifndef BEZIER_CURVE_HPP
#define BEZIER_CURVE_HPP

/**
 * @file BezierCurve.hpp
 * @brief Définition de la classe BezierCurve.
 * 
 * Ce fichier contient la représentation des courbes de Béziers et leur implémentation dans le
 * cadre d'un programme OpenGL.
 * 
 * @author Oscar G.
 * @date 2025-03-01
 */

#include <math.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.hpp"
#include "Curve.hpp"

#define DISCRETIZATION_STEP 0.01
#define EQUALY_BASE_SEGMENT 3.0 // Valeur qui semble correspondre après tests

/**
 * @class BezierCurve
 * @brief Objet représentant une courbe de Bézier et son polygone de contrôle.
 * 
 * Cette classe gère les points de contrôle et deux méthodes de discrétisation de la courbe.
 * Cette classe hérite de la classe Object, ce qui fait qu'elle est affichable via la fonction
 * draw() et son polygone de contrôle aussi.
 */
class BezierCurve : public Curve
{
public:

    /**
     * @brief Constructeur par défaut.
     * @param controlPoints Points de contrôle de la courbe de Bézier.
     */
    BezierCurve(vec3Array controlPoints,
        Transform transform = DEFAULT_OBJECT_TRANSFORM,
        Material material = DEFAULT_OBJECT_MATERIAL
    );

    /**
     * @brief Renvoie la valeur de la fonction définissant la courbe de Bézier B(u) pour une
     * valeur de u comprise dans l'intervalle [0;1].
     */
    glm::vec3 getValue(float u) override;

    /**
     * @brief Dessine la courbe de Bézier et son polygone de contrôle à l'écran avec OpenGL.
     * 
     * La courbe est dessinée en blanc et le polygone de contrôle est dessiné en rouge.
     * Cette fonction doit être appelée à dans la boucle d'affichage à chaque frame.
     */
    void draw(Scene* scene) override;
};

#endif //BEZIER_CURVE_HPP