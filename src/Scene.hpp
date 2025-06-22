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
    void setLightSource(glm::vec3 position);
    void addMesh(Mesh mesh);

private:
    std::vector<Mesh> m_meshes;
    glm::vec3 m_lightSource;
};

#endif // SCENE_HPP