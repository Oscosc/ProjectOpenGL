#include "Scene.hpp"
#include "Object.hpp"

Scene::Scene(Camera *camera, std::vector<Object *> objects) : m_objects(objects)
{
    this->m_cameras.push_back(camera);
    this->m_activeCamera = 0;
}

void Scene::render() {
    if(!camerasCount()) {
        std::cout << "[WARNING] No camera instanciated, scene render will be skipped" << std::endl;
        return;
    }

    updateActiveCameraPV();

    for(auto object : this->m_objects) {
        object->draw(this);
    }
}

void Scene::updateActiveCameraPV()
{
    this->m_activeCameraPV.projection = glm::perspective(
        glm::radians(getActiveCamera()->Zoom),
        getActiveCamera()->Ratio,
        0.1f,
        100.0f
    );
    this->m_activeCameraPV.view = getActiveCamera()->GetViewMatrix();
}

void Scene::updateLigth(Shader *shader)
{
    if(lightsCount() == 0) {
        std::cout << "[WARNING] No light source was instanciated, for somes shaders, nothing will be drawn" << std::endl;
        return;
    }

    shader->use();

    shader->setVec3("lightColor", getLight(0)->getColor());
    shader->setFloat("ambientStrength", getLight(0)->getStrength());
    shader->setFloat("specularStrength", getLight(0)->getStrength());
    
    shader->setVec3("lightPos", static_cast<PointLight*>(getLight(0))->getPosition());
}

void Scene::addCamera(Camera *camera)
{
    this->m_cameras.push_back(camera);
    if(camerasCount() == 1) { this->m_activeCamera = 0; }
}

void Scene::addObject(Object *object)
{
    this->m_objects.push_back(object);
}

void Scene::addLight(Light *light)
{
    this->m_lights.push_back(light);
}

Camera *Scene::getActiveCamera()
{
    return this->m_cameras.at(this->m_activeCamera);
}

ProjViewMatrix Scene::getActiveCameraPV()
{
    return this->m_activeCameraPV;
}
