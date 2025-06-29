#ifndef BEZIER_SURFACE
#define BEZIER_SURFACE

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

#include "Object.hpp"
#include "utils.hpp"

using ptsGrid = std::vector<std::vector<glm::vec3>>;

/**
 * @class BezierSurface
 * @brief Objet représentant une surface de Bézier et son polygone de contrôle.
 * 
 * Cette classe gère les points de contrôle et une méthode de discrétisation de la courbe.
 * Cette classe hérite de la classe Object, ce qui fait qu'elle est affichable via la fonction
 * draw().
 */
class BezierSurface : public Object
{
public:

    /**
     * @brief Constructeur par défaut.
     * @param control_points Points de contrôle de la surface de Bézier.
     */
    BezierSurface(ptsGrid control_points);
    glm::vec3 surfaceValue(float u, float v);

    void draw(Shader shader) override;

private:
    ptsGrid m_controlPoints;
    unsigned int m_sizeU;
    unsigned int m_sizeV;
    unsigned int m_nbCurvePointsU;
    unsigned int m_nbCurvePointsV;
    unsigned int m_nbVertices;

    unsigned int EBO;

    ptsGrid normalDiscretization();
    ptsGrid computeNormals(const ptsGrid& sampledPoints);
    void gridToGlfwDisplayableRepresentation(const ptsGrid &points, const ptsGrid &normals,
        ptsTab &tableVBO, std::vector<unsigned int> &tableEBO);
};

#endif // BEZIER_SURFACE
