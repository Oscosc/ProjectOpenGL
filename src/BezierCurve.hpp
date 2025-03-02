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

#define MIN_DISCRETE_POINTS 2
#define DISCRETIZATION_STEP 0.01
#define EQUALY_BASE_SEGMENT 3.0 // Valeur qui semble correspondre après tests

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils.hpp"
#include "Object.hpp"
#include "ScalableElement.hpp"

#include <math.h>
#include <iostream>
#include <vector>

/**
 * @class BezierCurve
 * @brief Objet représentant une courbe de Bézier et son polygone de contrôle.
 * 
 * Cette classe gère les points de contrôle et deux méthodes de discrétisation de la courbe.
 * Cette classe hérite de la classe Object, ce qui fait qu'elle est affichable via la fonction
 * draw() et son polygone de contrôle aussi.
 * Cette classe hérite de la classe ScalableElement, ce qui fait qu'elle peut être associée à un
 * contexte et modifiée en temps réel au travers des callbacks.
 */
class BezierCurve : public Object, public ScalableElement
{
public:

    /**
     * @brief Constructeur par défaut.
     * @param control_points Points de contrôle de la courbe de Bézier.
     */
    BezierCurve(ptsTab control_points);

    /**
     * @brief Affiche les coordonnées des points de contrôle de la courbe dans le terminal.
     */
    void debugControlPoints();

    /**
     * @brief Renvoie la valeur de la fonction définissant la courbe de Bézier B(u) pour une
     * valeur de u comprise dans l'intervalle [0;1].
     */
    glm::vec3 curveValue(float u);
    
    // ------------------------ FONCTIONS VIRTUELLES DE LA CLASSE "OBJECT" ------------------------

    /**
     * @brief Dessine la courbe de Bézier et son polygone de contrôle à l'écran avec OpenGL.
     * 
     * La courbe est dessinée en blanc et le polygone de contrôle est dessiné en rouge.
     * Cette fonction doit être appelée à dans la boucle d'affichage à chaque frame.
     * 
     * @param shader Shader utilisé pour faire le rendu de la courbe.
     * Attention : le shader doit implémenter la valeur uniforme "color" car la fonction y fait
     * référence pour colorer la courbe et le polygone de contrôle.
     */
    void draw(Shader shader) override;

    // ------------------- FONCTIONS VIRTUELLES DE LA CLASSE "SCALABLE_ELEMENT" -------------------

    /**
     * @brief Augmente la valeur de nbCurvePoints et recalcule les points à échantilloner sur la
     * courbe pour l'affichage et mets à jour les buffers.
     */
    void next() override;

    /**
     * @brief Diminue la valeur de nbCurvePoints et recalcule les points à échantilloner sur la
     * courbe pour l'affichage et mets à jour les buffers.
     */
    void previous() override;

    /**
     * @brief Permet de basculer entre le mode de discrétisation "normal" et le mode "equal".
     * Recalcule les points à échantilloner sur la courbe pour l'affichage et mets à jour les
     * buffers.
     */
    void switchMode() override;

private:

    ptsTab m_controlPoints;
    ptsTab m_curvePoints;
    unsigned int m_nbCurvePoints;

    GLuint controlVAO, controlVBO;

    /**
     * @brief Discrétise la courbe de Bézier avec nbCurvePoints points en prenant des intervalles
     * réguliers sur [0;1].
     * @return La liste des points qui forment la courbe discrétisée.
     */
    ptsTab normalDiscretization();

    /**
     * @brief Discrétise la courbe de Bézier de manière à avoir des segments de la même taille
     * graphiquement, cette taille étant de (EQUALY_BASE_SEGMENT / nbCurvePoints). Seul le dernier
     * segment a une longueur inférieure à cette valeur.
     * @return La liste des points qui forment la courbe discrétisée.
     */
    ptsTab equalDiscretization();

    /**
     * @brief Met à jour les points de la courbe en recalculant les valeurs discrètes avec la
     * fonction equalDiscretization() ou normalDiscretization() selon le mode actif (cf. classe
     * ScalableElement).
     * 
     * Cette fonction fait appel à la fonction parente de la classe objet pour mettre à jour le
     * buffer avec les nouvelles valeurs discrètes.
     */
    void updateCurvePoints();

    /**
     * @brief Calcule la valeur du polynôme de Bernstein au point u en tenant compte de n et i.
     * 
     * https://fr.wikipedia.org/wiki/Polyn%C3%B4me_de_Bernstein 
     */
    float bersteinValue(float u, int i, int n);
};

#endif //BEZIER_CURVE_HPP