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


class Mesh
{
public:
    enum LineType {COMMENT, VERTEX, FACET, NONE};

    Mesh(std::string file);
    void draw(Shader shader);

private:
    LineType identify(std::string token);

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    std::vector<glm::vec3> m_vertices;
    std::vector<unsigned int> m_indexes;

    glm::vec3 m_origin;
};

#endif // MESH_HPP