#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../includes/camera.hpp"
#include "../includes/shader.hpp"
#include "Light.hpp"
#include "PointLight.hpp"
#include "ProjViewMatrix.hpp"

class Object;

class Scene
{
public:
    Scene() {}
    Scene(Camera* camera);
    Scene(Camera* camera, std::vector<Object*> objects);
    ~Scene() = default;

    void render();
    void updateActiveCameraPV();
    void updateLigth(Shader* shader);

    Camera* getActiveCamera();
    ProjViewMatrix getActiveCameraPV();

    void addCamera(Camera* camera);
    void addObject(Object* object);
    void addLight(Light* light);

    const unsigned int camerasCount() { return m_cameras.size(); }
    const unsigned int lightsCount() { return m_lights.size(); }
    const unsigned int objectsCount() { return m_objects.size(); }

    Camera* getCamera(unsigned int index) { return m_cameras.at(index); }
    Light* getLight(unsigned int index) { return m_lights.at(index); }
    Object* getObject(unsigned int index) { return m_objects.at(index); }

private:
    unsigned int m_activeCamera;
    ProjViewMatrix m_activeCameraPV;

    std::vector<Camera*> m_cameras;
    std::vector<Object*> m_objects;
    std::vector<Light*> m_lights;

};