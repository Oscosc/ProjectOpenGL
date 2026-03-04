#include <ProjectIGAI/graphics/Object.hpp>

#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/ProjViewMatrix.hpp>
#include <ProjectIGAI/core/Scene.hpp>

Object::Object(const std::string& name) : m_geometry(nullptr), m_material(nullptr), Node(DEFAULT_TRANSFORM, name)
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

void Object::draw(Scene *scene, glm::mat4 parentTransform)
{
    glm::mat4 globalTransform = parentTransform * getLocalModelMatrix();

    // Animation retrieving ---------------------
    Animator* animator = nullptr;
    Node* currentNode = this;

    while (currentNode != nullptr) {
        if (currentNode->getAnimator() != nullptr) {
            animator = currentNode->getAnimator();
            break;
        }
        currentNode = const_cast<Node*>(currentNode->getParent());
    }
    // ------------------------------------------

    Shader* shader = m_material->getShader();
    if (shader)
    {
        shader->use();

        if (animator) {
            auto transforms = animator->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i) {
                shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            }
            shader->setBool("hasBones", true); 
        } else {
            shader->setBool("hasBones", false);
        }

        if (m_material && m_geometry)
        {
            m_material->bind(scene);

            ProjViewMatrix pv = scene->getActiveCameraPV();
            shader->setMat4("view", pv.view);
            shader->setMat4("projection", pv.projection);
            shader->setMat4("model", globalTransform);

            m_geometry->draw();
        }
    }

    // Node propagation
    for (Node* child : m_childrens)
    {
        child->draw(scene, globalTransform);
    }
}
