#ifndef OBJECT_HPP
#define OBJECT_HPP


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>


class Object
{
public:
    unsigned int VAO, VBO;
    std::vector<glm::vec3> points;

    Object(std::vector<glm::vec3> points)
    {
        this->points = points; 

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }

    Object(std::vector<glm::vec3> points, unsigned int forced_buffer_size)
    {
        this->points = points; 

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, forced_buffer_size * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void updateVertices(std::vector<glm::vec3> new_points)
    {
        this->points = new_points;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), points.data());
    }

    void drawLine()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());
    }

    void deleteBuffers()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

private:
};

#endif // OBJECT_HPP