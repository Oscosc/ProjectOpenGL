#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

/** Names to easily identify arrays/matrix of vectors */
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