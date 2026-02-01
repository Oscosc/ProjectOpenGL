#pragma once

#include <glm/glm.hpp>

#define DEFAULT_TRANSFORM {glm::vec3(0.0), glm::vec3(1.0), glm::vec3(0.0)}
#define DEFAULT_NAME "None"

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
    virtual ~Node() = 0;

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

protected:

    /** Transformation of the node element */
    Transform m_transform;

    /** Name of the node */
    std::string m_name;

};

/**
 * @brief Virtual destructor declaration for compilation purposes.
 */
inline Node::~Node() {}