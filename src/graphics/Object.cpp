#include <ProjectIGAI/graphics/Object.hpp>

#include <ProjectIGAI/core/Logger.hpp>

Object::Object(const std::string& name) : m_geometry(nullptr), Node(DEFAULT_TRANSFORM, name)
{
}

Object::~Object()
{
}

void Object::setGeometry(Geometry *geometry)
{
    if(!geometry)
        Logger::logWarning("Trying to attach an empty geometry to an object");
    else
        m_geometry = geometry;
}

Geometry *Object::getGeometry() const
{
    return m_geometry;
}

void Object::setMaterial(Material *material)
{
    if(!material)
        Logger::logWarning("Trying to attach an empty material to an object");
    else
        m_material = material;
}

Material *Object::getMaterial() const
{
    return m_material;
}

void Object::draw(Scene *scene)
{
    m_material->bind(scene);
    m_geometry->draw();
}
