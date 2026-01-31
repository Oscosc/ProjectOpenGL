#pragma once

#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/core/Logger.hpp>

#define NB_CURVE_POINTS 30
#define DEFAULT_CONTROL_COLOR {1.f, 0.f, 0.f}
#define DEFAULT_CONTROL_SIZE 5.f

/**
 * @brief General class to define a curve. This abstraction can be used to define a Bezier Curve
 * or a B-Spline Curve.
 * 
 */
class Curve : public Object
{
public:
    /** Visibility of a control polygon (invisible, only points or complete curve/lines) */
    enum Visibility {OFF, POINTS, LINES};

    /**
     * @brief Construct a new Curve object based on it's transformation and it's material
     * 
     * @param transform transformation of the curve
     * @param material material of the curve
     */
    Curve(Transform transform, std::string name, Material material) : Object(transform, name, material) {}

    /**
     * @brief Get the position of a point based on a curve value.
     * 
     * @param u curve index value
     * @return position of the associated point
     */
    virtual glm::vec3 getValue(float u) const = 0;

protected:

    /**
     * @brief Function used to draw the control polygon of the curve.
     * 
     */
    void drawControlPolygon() const;

    /**
     * @brief Init the control polygon buffers, since it's separated from the curve
     * buffers itself.
     * 
     */
    void initGLControlPolygon();

    /** VAO and VBO for control polygon */
    unsigned int m_controlVAO;
    unsigned int m_controlVBO;

    /** List of control points and control polygon visibility */
    vec3Array m_controlPoints;
    Visibility m_controlPolygonVisibility;
};

/**
 * @brief Renvoie la valeur du triangle de Pascal pour un i et un n donnés en paramètre.
 */
int PascalValue(int i, int n);

/**
 * @brief Calcule la valeur du polynôme de Bernstein au point u en tenant compte de n et i.
 * 
 * https://fr.wikipedia.org/wiki/Polyn%C3%B4me_de_Bernstein 
 */
float bersteinValue(float u, int i, int n);

std::vector<Vertex> defaultPointToVertex(const vec3Array& points);