#pragma once

#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/core/utils.hpp>

class Surface : public Object
{
public:
    Surface(Transform transform, Material material) : Object(transform, material) {}
    virtual glm::vec3 getValue(glm::vec2 uv) const = 0;

protected:
    void drawControlPolygon() const;
    void initGLControlPolygon();

    unsigned int m_controlVAO;
    unsigned int m_controlVBO;

    vec3Grid m_controlPoints;
    unsigned int m_sizeU;
    unsigned int m_sizeV;
};