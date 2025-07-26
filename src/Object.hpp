#pragma once

#include "../includes/shader.hpp"
#include "Scene.hpp"
#include "ProjViewMatrix.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

#define DEFAULT_OBJECT_TRANSFORM {glm::vec3(0.0), glm::vec3(1.0), glm::vec3(0.0)}
#define DEFAULT_OBJECT_MATERIAL {ShaderManager::getInstance().getShader("monochrome"), {glm::vec3(0.5), glm::vec3(0.5), glm::vec3(0.5), 32.f}}

using vec3Array = std::vector<glm::vec3>;
using vec2Array = std::vector<glm::vec2>;
using vec3Grid = std::vector<std::vector<glm::vec3>>;

/**
 * @brief Complete representation of a vertice in a graphic sense.
 * Contain position, normal, uv and equality operator.
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    /**
     * @brief Implementation of the equality operator for Vertices.
     * 
     * Vertices are equals if all their attributes are equals.
     * 
     * @param other Vertice to compare with
     */
    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

/**
 * @brief Specialization of the hash function for the Vertex structure to allow
 * Vertices to be added to an unordered map (computeUniques function)
 */
namespace std {
    template <>
    struct hash<Vertex> {
        std::size_t operator()(const Vertex& v) const {
            std::size_t hPos = std::hash<float>()(v.position.x)
                ^ std::hash<float>()(v.position.y)
                ^ std::hash<float>()(v.position.z);
            std::size_t hNorm = std::hash<float>()(v.normal.x)
                ^ std::hash<float>()(v.normal.y)
                ^ std::hash<float>()(v.normal.z);
            std::size_t hUV = std::hash<float>()(v.uv.x)
                ^ std::hash<float>()(v.uv.y);
                
            return hPos ^ (hNorm << 1) ^ (hUV << 2);
        }
    };
}

/**
 * @brief Intermediate structure for representing Vertex indices (one index per vertex attribute).
 * 
 */
struct VertexIndex {
    int position;
    int normal;
    int uv;
};

struct ShaderMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

/**
 * @brief Structure that contains informations about an object material.
 */
struct Material {
    Shader* shader;
    ShaderMaterial matShader;
};

/**
 * @brief Structure that contains informations about an object transformation.
 */
struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

/**
 * @brief This abstract class is defining any type of object that can be represented visualy in
 * a 3D world, with transformation and material.
 */
class Object
{
public:

    /**
     * @brief Construct a general object by defining his default values.
     * 
     * @param transform transformation of the object in the 3D world
     * @param material material of the object used to render it
     */
    Object(
        Transform transform = DEFAULT_OBJECT_TRANSFORM,
        Material material = DEFAULT_OBJECT_MATERIAL
    ) : m_transform(transform), m_material(material) {}

    /**
     * @brief Default destructor for object class.
     */
    virtual ~Object() = default;

    /**
     * @brief draw this object in the scene gived in argument.
     * 
     * @param scene Scene where this object will be rendered
     */
    virtual void draw(Scene* scene) = 0;

    /**
     * @brief Give the material of this object.
     */
    Material getMaterial() { return m_material; }

    /**
     * @brief Set a new material for this object.
     * 
     * @param material new material
     */
    void setMaterial(Material material) { m_material = material; }

    /**
     * @brief Give the transformation state of this object.
     */
    Transform getTransform() { return m_transform; }

    /**
     * @brief Set a new transformation for this object.
     * 
     * @param transform new transform
     */
    void setTransform(Transform transform) { m_transform = transform; }

    void debugMaterial();

protected:

    void initGLObject();
    void updateMaterial(Shader* shader);

    Transform m_transform;
    Material m_material;

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    bool m_hasNormals;
    bool m_hasUVs;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indexes;
};