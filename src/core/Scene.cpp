#include <ProjectIGAI/core/Scene.hpp>

#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/graphics/GeometryManager.hpp>

void Scene::render() {
    if(!camerasCount()) {
        Logger::logWarning("No camera instanciated, scene render will be skipped");
        return;
    }

    updateActiveCameraPV();

    for(auto object : this->m_nodes) {
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
    if(lightsCount().length == 0) {
        Logger::logWarning("No light source was instanciated, for somes shaders, nothing will be drawn");
        return;
    }

    shader->use();

    unsigned int i = 0;
    for(PointLight* light : m_lights.pointLights) {
        std::string i_str = std::to_string(i);
        shader->setVec3("pointLights[" + i_str + "].color", light->getLightMaterial().color);
        shader->setFloat("pointLights[" + i_str + "].intensity", light->getLightMaterial().intensity);
        shader->setFloat("pointLights[" + i_str + "].radius", light->getRadius());
        shader->setVec3("pointLights[" + i_str + "].position", light->getTransform().position);
        ++i;
    }

    i = 0;
    for(DirectionalLight* light : m_lights.dirLights) {
        std::string i_str = std::to_string(i);
        shader->setVec3("dirLights[" + i_str + "].color", light->getLightMaterial().color);
        shader->setFloat("dirLights[" + i_str + "].intensity", light->getLightMaterial().intensity);
        shader->setVec3("dirLights[" + i_str + "].direction", light->getForwardVector());
        ++i;
    }

    i = 0;
    for(SpotLight* light : m_lights.spotLights) {
        std::string i_str = std::to_string(i);
        shader->setVec3("spotLights[" + i_str + "].color", light->getLightMaterial().color);
        shader->setFloat("spotLights[" + i_str + "].intensity", light->getLightMaterial().intensity);
        shader->setVec3("spotLights[" + i_str + "].position", light->getTransform().position);
        shader->setFloat("spotLights[" + i_str + "].radius", light->getRadius());
        shader->setVec3("spotLights[" + i_str + "].direction", light->getForwardVector());
        shader->setFloat("spotLights[" + i_str + "].innerCos", light->getCutOff());
        shader->setFloat("spotLights[" + i_str + "].outerCos", light->getOuterCutOff());
        ++i;
    }

    shader->setVec3("viewPos", this->getActiveCamera()->Position);
}

const glm::vec3 Scene::lightsCount() const {
    return glm::vec3(m_lights.pointLights.size(), m_lights.dirLights.size(), m_lights.spotLights.size());
}

Light *Scene::getLight(unsigned int index, unsigned int type) const
{
    switch (type)
    {
    case POINT_LIGHT_INDEX: return m_lights.pointLights.at(index);
    case DIR_LIGHT_INDEX:   return m_lights.dirLights.at(index);
    case SPOT_LIGHT_INDEX:  return m_lights.spotLights.at(index);
    default: return nullptr;
    }
}

Camera *Scene::getActiveCamera() const
{
    return this->m_cameras.at(this->m_activeCamera);
}

ProjViewMatrix Scene::getActiveCameraPV() const
{
    return this->m_activeCameraPV;
}
