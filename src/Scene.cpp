#include "Scene.hpp"

Scene::Scene()
{
}

void Scene::draw(Shader shader)
{
    for(auto mesh : this->m_meshes) {
        mesh.draw(shader);
    }
}

void Scene::setLightSource(glm::vec3 position)
{
    this->m_lightSource = position;
}

void Scene::addMesh(Mesh mesh)
{
    this->m_meshes.push_back(mesh);
}
