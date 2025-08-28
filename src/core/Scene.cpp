#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/graphics/Sphere.hpp>
#include <ProjectIGAI/core/Logger.hpp>

Scene::Scene(Camera *camera, std::vector<Object *> objects) : m_objects(objects)
{
    this->m_cameras.push_back(camera);
    this->m_activeCamera = 0;
}

void Scene::render() {
    if(!camerasCount()) {
        Logger::logWarning("No camera instanciated, scene render will be skipped");
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
        glm::radians(getActiveCamera()->Fov),
        getActiveCamera()->Ratio,
        0.1f,
        100.0f
    );
    this->m_activeCameraPV.view = getActiveCamera()->GetViewMatrix();
}

void Scene::updateLigth(Shader *shader)
{
    if(lightsCount() == 0) {
        Logger::logWarning("No light source was instanciated, for somes shaders, nothing will be drawn");
        return;
    }

    shader->use();

    unsigned int i = 0;
    for(PointLight* light : m_lights.pointLights) {
        std::string i_str = std::to_string(i);
        shader->setVec3("pointLights[" + i_str + "].ambient", light->getLightMaterial().ambient);
        shader->setVec3("pointLights[" + i_str + "].diffuse", light->getLightMaterial().diffuse);
        shader->setVec3("pointLights[" + i_str + "].specular", light->getLightMaterial().specular);
        shader->setVec3("pointLights[" + i_str + "].position", light->getPosition());
        ++i;
    }

    i = 0;
    for(DirectionalLight* light : m_lights.dirLights) {
        std::string i_str = std::to_string(i);
        shader->setVec3("dirLights[" + i_str + "].ambient", light->getLightMaterial().ambient);
        shader->setVec3("dirLights[" + i_str + "].diffuse", light->getLightMaterial().diffuse);
        shader->setVec3("dirLights[" + i_str + "].specular", light->getLightMaterial().specular);
        shader->setVec3("dirLights[" + i_str + "].direction", light->getDirection());
        ++i;
    }

    i = 0;
    for(SpotLight* light : m_lights.spotLights) {
        std::string i_str = std::to_string(i);
        shader->setVec3("spotLights[" + i_str + "].ambient", light->getLightMaterial().ambient);
        shader->setVec3("spotLights[" + i_str + "].diffuse", light->getLightMaterial().diffuse);
        shader->setVec3("spotLights[" + i_str + "].specular", light->getLightMaterial().specular);
        shader->setVec3("spotLights[" + i_str + "].position", light->getPosition());
        shader->setVec3("spotLights[" + i_str + "].direction", light->getDirection());
        shader->setFloat("spotLights[" + i_str + "].cutOff", light->getCutOff());
        shader->setFloat("spotLights[" + i_str + "].outerCutOff", light->getOuterCutOff());
        ++i;
    }

    shader->setVec3("viewPos", this->getActiveCamera()->Position);
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

const unsigned int Scene::lightsCount() const {
    return m_lights.dirLights.size() + m_lights.pointLights.size() + m_lights.spotLights.size();
}

std::vector<Sphere*> Scene::getSpheresRT() const
{
    std::vector<Sphere*> spheres;
    for(auto obj : m_objects) {
        Sphere* s = dynamic_cast<Sphere*>(obj);
        if(s != nullptr) spheres.push_back(s);
    }
    return spheres;
}

Camera *Scene::getActiveCamera() const
{
    return this->m_cameras.at(this->m_activeCamera);
}

ProjViewMatrix Scene::getActiveCameraPV() const
{
    return this->m_activeCameraPV;
}
