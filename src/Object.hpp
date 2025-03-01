#ifndef OBJECT_HPP
#define OBJECT_HPP


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>


using ptsTab = std::vector<glm::vec3>;


/**
 * @class Object
 * @brief Classe abstraite pour les objets qui doivent être rendus en OpenGL.
 */
class Object
{
public:

    Object();
    virtual ~Object();
    virtual void draw() = 0; // A redéfinir pour chaque objet

protected:

    GLuint VAO, VBO;

    void updateVertices(ptsTab points);
};

#endif // OBJECT_HPP