#pragma once

#include <ProjectIGAI/graphics/Node.hpp>
#include <ProjectIGAI/geometry/Geometry.hpp>
#include <ProjectIGAI/graphics/Material.hpp>

class Object : public Node
{
public:

    Object(const std::string& name = "Object");

    virtual ~Object();

    void setGeometry(Geometry* geometry);

    Geometry* getGeometry() const;

    void setMaterial(Material* material);

    Material* getMaterial() const;

    void draw(Scene* scene);

private:

    Geometry* m_geometry;
    Material* m_material;
};