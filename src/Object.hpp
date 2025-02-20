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

    Object(std::vector<glm::vec3> points);
    Object(std::vector<glm::vec3> points, unsigned int forced_buffer_size);

    void updateVertices(std::vector<glm::vec3> new_points);
    void drawLine();
    void deleteBuffers();

private:

    unsigned int VAO, VBO;
    std::vector<glm::vec3> points;
};

#endif // OBJECT_HPP