#pragma once

#include <extern/shader.hpp>
#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/ProjViewMatrix.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/TextureManager.hpp>
#include <ProjectIGAI/graphics/Node.hpp>

#define DEFAULT_OBJECT_MATERIAL {ShaderManager::getInstance().getShader("monochrome"), {glm::vec3(0.5), 1.0f, 0.0f}}

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
    glm::vec3 color;
    float roughness;
    float metallic;
};

/**
 * @brief Structure that contains informations about an object material.
 */
struct Material {
    Shader* shader;
    ShaderMaterial matShader;
};

/**
 * @brief This abstract class is defining any type of object that can be represented visualy in
 * a 3D world, with transformation and material.
 */
class Object : public Node
{
public:

    /**
     * @brief Construct a general object by defining his default values.
     * 
     * @param transform transformation of the object in the 3D world
     * @param material material of the object used to render it
     */
    Object(
        Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        Material material = DEFAULT_OBJECT_MATERIAL)
    : Node(transform, name), m_material(material) {}

    /**
     * @brief Default destructor for object class.
     */
    virtual ~Object() = default;

    /**
     * @brief draw this object in the scene gived in argument.
     * 
     * @param scene Scene where this object will be rendered
     */
    virtual void draw(Scene* scene) const = 0;

    /**
     * @brief Give the material of this object.
     */
    Material getMaterial() const { return m_material; }

    /**
     * @brief Set a new material for this object.
     * 
     * @param material new material
     */
    void setMaterial(Material material) { m_material = material; }

    /**
     * @brief Print debug informations of the material in console.
     * 
     */
    void debugMaterial() const;

    /**
     * @brief Get object's Model Matrix.
     * 
     * @return glm::mat4 
     */
    glm::mat4 getModelMatrix() const;

protected:

    /**
     * @brief init the object as an OpenGL object (buffers, etc...)
     * 
     */
    void initGLObject();

    /**
     * @brief Update the material informations in the shader based on
     * object's material informations.
     * 
     * @param shader shader to modify.
     */
    void updateMaterial(Shader* shader) const;

    /** Material of the object */
    Material m_material;

    /** Various buffers for OpenGL */
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    /** Information about the mesh configuration */
    bool m_hasNormals;
    bool m_hasUVs;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indexes;
};