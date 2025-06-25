#include "Scene.hpp"

Scene::Scene()
{
}

void Scene::draw(Shader shader)
{
    shader.use();
    shader.setVec3("lightPos", this->m_lightSource);
    shader.setVec3("lightColor", this->m_lightColor);
    shader.setFloat("ambientStrength", 0.4f);
    shader.setFloat("specularStrength", 0.5f);

    for(Mesh mesh : this->m_meshes) {
        mesh.draw(shader);
    }
}

void Scene::addMesh(Mesh mesh)
{
    this->m_meshes.push_back(mesh);
}

void Scene::setLightSource(glm::vec3 position)
{
    this->m_lightSource = position;
}

glm::vec3 Scene::getLightSource()
{
    return this->m_lightSource;
}

void Scene::setLightColor(glm::vec3 color)
{
    this->m_lightColor = color;
}

glm::vec3 Scene::getLightColor()
{
    return this->m_lightColor;
}