#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils.hpp"
#include "../includes/shader.hpp"

#define STD_DELIMITER " "

using vec3Array = std::vector<glm::vec3>;
using vec2Array = std::vector<glm::vec2>;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct VertexIndex {
    unsigned int position;
    unsigned int normal;
    unsigned int uv;
};

class Mesh
{
public:
    enum LineType {
        COMMENT,
        POSITION,
        NORMAL,
        UV,
        INDEX,
        NONE
    };

    Mesh(std::string file);
    void draw(Shader shader);

    bool hasNormals();
    bool hasUVs();

private:
    LineType identify(std::string token);
    
    void parseAsData(LineType id, std::vector<std::string> tokens, vec3Array &positions, vec3Array &normals, vec2Array &uvs);
    void parseAsIndexes(LineType id, std::vector<std::string> tokens, std::vector<VertexIndex> &indexes);
    void computeUniques(vec3Array &positions, vec3Array &normals, vec2Array &uvs, std::vector<VertexIndex> &indexes);

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    bool m_hasNormals;
    bool m_hasUVs;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indexes;

    glm::vec3 m_origin;
};

#endif // MESH_HPP