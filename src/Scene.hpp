#ifndef SCENE_HPP
#define SCENE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "Mesh.hpp"
#include "../includes/shader.hpp"

class Scene
{
public:
    Scene();
    void draw(Shader shader);
    void addMesh(Mesh mesh);
    void setLightSource(glm::vec3 position);
    glm::vec3 getLightSource();
    void setLightColor(glm::vec3 color);
    glm::vec3 getLightColor();

private:
    std::vector<Mesh> m_meshes;
    glm::vec3 m_lightSource;
    glm::vec3 m_lightColor;
};

#endif // SCENE_HPP