#pragma once

#include <string>
#include <vector>

#include <ProjectIGAI/geometry/Vertex.hpp>

#define STD_DELIMITER " "
#define IDX_DELIMITER "/"

/**
 * @brief Enumeration for representing types of lines in a .obj file
 * 
 * This enumeration is used to call the right function in the parsing context.
 */
enum LineType {
    COMMENT,    // Starts with '#'
    POSITION,   // Starts with 'v'
    NORMAL,     // Starts with 'vn'
    UV,         // Starts with 'vt'
    INDEX,      // Starts with 'f'
    NONE        // Default value
};


class ObjLoader
{
public:

    static void loadObject(std::string path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);

    static LineType identifyObjLineType(std::string token);

    /**
     * @brief Parses the line as a line with data information
     * (position, normal, or UV). Updates the list corresponding to the identified line type.
     *
     * WARNING: It is assumed that the function is called correctly based on the parsed ID.
     * Otherwise, the behavior is undefined.
     *
     * @param id Line identifier (token 0)
     * @param tokens List of tokens in the line (including token 0!)
     * @param positions List of positions already read since the beginning of mesh construction
     * @param normals List of normals already read since the beginning of mesh construction
     * @param uvs List of UVs already read since the beginning of mesh construction
     */
    static void parseAsData(const LineType id, const std::vector<std::string> tokens,
        vec3Array &positions, vec3Array &normals, vec2Array &uvs);

    /**
     * @brief Parses the line as a line with index information.
     * Updates the list of indexes passed as a parameter for future construction.
     *
     * @param id Line identifier (token 0)
     * @param tokens List of tokens in the line (including token 0!)
     * @param indexes List of indexes already read since the start of mesh construction
     */
    static void parseAsIndexes(const LineType id, const std::vector<std::string> tokens,
        std::vector<VertexIndex> &indexes);

    static void subComputeNormals(const vec3Array &positions, vec3Array &normals,
        std::vector<VertexIndex> &indexes, const bool invertNormals = false);

    static void computeUniques(
        const vec3Array &positions,
        const vec3Array &normals,
        const vec2Array &uvs,
        const std::vector<VertexIndex> &indexes,
        std::vector<Vertex>& vertexBuffer,
        std::vector<unsigned int>& elementBuffer
    );
};