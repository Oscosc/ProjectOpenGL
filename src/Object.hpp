#pragma once

#include "../includes/shader.hpp"
#include "Scene.hpp"
#include "ProjViewMatrix.hpp"
#include "ShaderManager.hpp"

using vec3Array = std::vector<glm::vec3>;
using vec2Array = std::vector<glm::vec2>;

/**
 * @brief Représentation complète d'un vertex au sens graphique.
 * Encapsule la position, la normale et les UVs, ainsi que l'opérateur d'égalité.
 * 
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    /**
     * @brief Implémentation de l'opérateur d'égalité pour les Vertex
     * 
     * Si la position, la normale et les UV sont égaux, alors les Vertex sont égaux. Sinon non.
     * 
     * @param other Vertex à comparer
     * @return true si les deux Vertex sont les mêmes, false sinon
     */
    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

/**
 * @brief Spécialisation de la fonction de hash pour la structure Vertex afin de permettre aux
 * Vertex d'être ajouté dans une unordered map (fonction computeUniques)
 * 
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
 * @brief Représentation d'un indexe pour un vertex. Cette structure est une structure
 * intermédiaire avant de former une liste de Vertex qui sera passée au VAO.
 * 
 */
struct VertexIndex {
    int position;
    int normal;
    int uv;
};

struct Material {
    Shader* shader;
    glm::vec3 color;
};

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

class Object
{
public:
    Object(Transform transform) :
        m_transform(transform),
        m_material({ShaderManager::getInstance().getShader("monochrome"), glm::vec3(1.0f)}) {}
    Object(Transform transform, Material material) : m_transform(transform), m_material(material) {}
    virtual ~Object() = default;

    virtual void draw(Scene* scene) = 0;

    Material getMaterial() { return m_material; }
    void setMaterial(Material material) { m_material = material; }

    Transform getTransform() { return m_transform; }
    void setTransform(Transform transform) { m_transform = transform; }

protected:
    void initGLObject();

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