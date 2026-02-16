#pragma once

#include <extern/shader.hpp>
#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/ProjViewMatrix.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/TextureManager.hpp>
#include <ProjectIGAI/graphics/Node.hpp>
#include <ProjectIGAI/geometry/Geometry.hpp>

#define DEFAULT_OBJECT_MATERIAL {ShaderManager::getInstance().getResource("monochrome"), {glm::vec3(0.5), 1.0f, 0.0f}}

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
    : Node(transform, name), m_material(material), m_geometry(nullptr) {}

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

    void addTexture(std::string name);
    
    /**
     * @brief If this object has a texture, bind it with the shader.
     * 
     */
    void bindTexture(Shader* shader, Scene* scene) const;

protected:

    /**
     * @brief Update the material informations in the shader based on
     * object's material informations.
     * 
     * @param shader shader to modify.
     */
    void updateMaterial(Shader* shader) const;

    /** Material of the object */
    Material m_material;

    /** Information about the mesh configuration */
    bool m_hasNormals;
    bool m_hasUVs;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indexes;

    bool m_hasTexture = false;
    unsigned int m_texture;

    Geometry* m_geometry;
};