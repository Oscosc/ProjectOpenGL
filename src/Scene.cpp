#include "Scene.hpp"

Scene::Scene()
{
}

void Scene::draw(Shader shader)
{
    shader.use();
    shader.setVec3("lightPos", this->m_lightSource);
    shader.setVec3("lightColor", glm::vec3(1.0f));
    shader.setFloat("ambientStrength", 0.4f);
    for(Mesh mesh : this->m_meshes) {
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
