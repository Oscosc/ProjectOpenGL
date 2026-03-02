#pragma once

#include <ProjectIGAI/graphics/Node.hpp>
#include <ProjectIGAI/geometry/Geometry.hpp>
#include <ProjectIGAI/graphics/Material.hpp>

class Scene;

class Object : public Node
{
public:

    Object(const std::string& name = "Object");

    virtual ~Object();

    void setGeometry(Geometry* geometry);

    Geometry* getGeometry() const;

    void setMaterial(Material* material);

    Material* getMaterial() const;

    void draw(Scene* scene, glm::mat4 parentTransform = glm::mat4(1.0f)) override;

private:

    Geometry* m_geometry;
    Material* m_material;
};