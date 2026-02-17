#include <ProjectIGAI/graphics/Object.hpp>

#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/ProjViewMatrix.hpp>
#include <ProjectIGAI/core/Scene.hpp>

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
    // Material setup
    m_material->bind(scene);

    // Transformation setup
    Shader* shader = m_material->getShader();
    if (!shader) return;
    shader->use();

    ProjViewMatrix pv = scene->getActiveCameraPV();
    shader->setMat4("view", pv.view);
    shader->setMat4("projection", pv.projection);

    Transform t = this->getTransform();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, t.position);
    model = glm::rotate(model, glm::radians(t.rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(t.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(t.rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, t.scale);
    shader->setMat4("model", model);

    // Drawing the geometry using shader
    m_geometry->draw();
}
