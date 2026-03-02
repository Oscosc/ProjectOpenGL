#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

#define DEFAULT_TRANSFORM {glm::vec3(0.0), glm::vec3(1.0), glm::vec3(0.0)}
#define DEFAULT_NAME "None"

class Scene;

/**
 * @brief Transformation of any element present in the world.
 */
struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

/**
 * @brief Root class for every object that influence the world in a way.
 * * Inheritance table :
 * ```text
 * Node (Tranform, Name)
 * ├── Object (Material, Vertices)
 * │   ├── Mesh (Reference file)
 * │   ├── Sphere (Radius)
 * │   ├── Curve (Control points)
 * │   │   └── Bezier Curve (Computation methods)
 * │   ├── Surface (Control polygon)
 * │   │   └── Bezier Surface (Computation methods)
 * │   └── Grid (...)
 * ├── Light (Properties)
 * ├── Point Light (Radius)
 * |   └──Spot Light (Outer/Inner cut-off)
 * └── Directional Light
 * ```
 */
class Node
{
public:

    /**
     * @brief Construct a new Node object.
     * 
     * @param transform transform of the Node. Default is :
     * position = {0, 0, 0}, scale = {1, 1, 1}, rotation = {0, 0, 0}.
     * @param name name of the Node. Default is 'None'.
     */
    Node(Transform transform = DEFAULT_TRANSFORM, std::string name = DEFAULT_NAME)
        : m_transform(transform), m_name(name) {}

    /**
     * @brief default destructor (virtual to keep class abstract).
     */
    virtual ~Node() = default;

    /**
     * @brief Give the transformation state of this node.
     */
    const Transform& getTransform() const { return m_transform; }

    /**
     * @brief Set a new transformation for this node.
     * 
     * @param transform new transform
     */
    void setTransform(Transform transform) { m_transform = transform; }

    /**
     * @brief Give the name of this node.
     */
    const std::string& getName() const { return m_name; }

    void setName(const std::string name) { m_name = name; }

    /**
     * @brief Get the Forward Vector object
     * 
     * @return const glm::vec3& 
     */
    glm::vec3 getForwardVector() const
    {
        glm::vec3 worldForward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::quat rotationQuat = glm::quat(glm::radians(m_transform.rotation));
        return glm::normalize(rotationQuat * worldForward);
    }

    glm::mat4 getLocalModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_transform.position);
        model = glm::rotate(model, glm::radians(m_transform.rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(m_transform.rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(m_transform.rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, m_transform.scale);
        return model;
    }

    virtual void draw(Scene* scene, glm::mat4 parentTransform = glm::mat4(1.0f))
    {
        glm::mat4 globalTransform = parentTransform * getLocalModelMatrix();

        for (Node* child : m_childrens)
        {
            child->draw(scene, globalTransform);
        }
    }

    void addChildren(Node& child) {
        m_childrens.push_back(&child);
        child.setParent(*this);
    }

    const std::vector<Node*> getChildrens() { return m_childrens; }

    const Node* getParent() { return m_parent; }

    void setParent(Node& parent) { m_parent = &parent; }

    bool isRoot() { return m_parent == nullptr; }

protected:

    /** Transformation of the node element */
    Transform m_transform;

    Node* m_parent = nullptr;
    std::vector<Node*> m_childrens;

    /** Name of the node */
    std::string m_name;

};