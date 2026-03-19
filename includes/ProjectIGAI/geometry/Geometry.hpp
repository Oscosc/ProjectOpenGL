#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ProjectIGAI/geometry/Vertex.hpp>

class Geometry
{
public:

    Geometry(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indexes);

    ~Geometry();
    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;

    void draw();

private:

    /** Various buffers for OpenGL */
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    GLsizei m_indexCount;
};