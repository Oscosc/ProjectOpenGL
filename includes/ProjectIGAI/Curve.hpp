#pragma once

#include <ProjectIGAI/Object.hpp>
#include <ProjectIGAI/Logger.hpp>

#define NB_CURVE_POINTS 30
#define DEFAULT_CONTROL_COLOR {1.f, 0.f, 0.f}
#define DEFAULT_CONTROL_SIZE 5.f

inline std::vector<Vertex> defaultPointToVertex(const vec3Array& points);

class Curve : public Object
{
public:
    enum Visibility {OFF, POINTS, LINES};

    Curve(Transform transform, Material material) : Object(transform, material) {}
    virtual glm::vec3 getValue(float u) const = 0;

protected:
    void drawControlPolygon() const;
    void initGLControlPolygon();

    unsigned int m_controlVAO;
    unsigned int m_controlVBO;

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