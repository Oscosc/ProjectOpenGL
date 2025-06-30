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

    void addObject(Object* object);
    void addLight(Light* light);

    Camera* getActiveCamera();
    ProjViewMatrix getActiveCameraPV();

    Light* getLight(unsigned int index) { return m_lights.at(index); }
    Object* getObject(unsigned int index) { return m_objects.at(index); }

private:
    unsigned int m_activeCamera;
    ProjViewMatrix m_activeCameraPV;

    std::vector<Camera*> m_cameras;
    std::vector<Object*> m_objects;
    std::vector<Light*> m_lights;

};