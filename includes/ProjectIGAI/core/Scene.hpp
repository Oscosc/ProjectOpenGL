#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <extern/camera.hpp>
#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/Light.hpp>
#include <ProjectIGAI/graphics/PointLight.hpp>
#include <ProjectIGAI/graphics/DirectionalLight.hpp>
#include <ProjectIGAI/graphics/SpotLight.hpp>
#include <ProjectIGAI/graphics/ProjViewMatrix.hpp>

class Object;
class Sphere;

struct LightGroup {
    std::vector<PointLight*> pointLights;
    std::vector<DirectionalLight*> dirLights;
    std::vector<SpotLight*> spotLights;
};

/**
 * @brief Class representing a Scene which contain objects, lights and Cameras.
 */
class Scene
{
public:

    /**
     * @brief Empty constructor of Scene. Will throw a warning if no camera is defined before
     * rendering any object.
     */
    Scene() {}

    /**
     * @brief Construct a new Scene with a default camera and multiples pre-constructed objects.
     * 
     * @param camera default camera
     * @param objects list of scene objects
     */
    Scene(Camera* camera, std::vector<Object*> objects = std::vector<Object*>());

    /**
     * @brief Default destructor for Scene object.
     */
    ~Scene() = default;

    /**
     * @brief Render every object and lights in the scene in the point of view of the active
     * camera.
     */
    void render();

    /**
     * @brief Update the Point of View matrix in function of the active Camera.
     */
    void updateActiveCameraPV();

    /**
     * @brief Update the shader given in parameter with the light informations of the scene.
     * 
     * Currently : only the first light is taken in account
     */
    void updateLigth(Shader* shader);

    /**
     * @brief Return a reference to the active Camera of the scene
     */
    Camera* getActiveCamera() const;

    /**
     * @brief Return the active camera point of view matrix
     */
    ProjViewMatrix getActiveCameraPV() const;

    /**
     * @brief Add a new camera to the scene.
     * 
     * @param camera element to add.
     */
    void addCamera(Camera* camera);

    /**
     * @brief Add a new object to the scene.
     * 
     * @param object element to add.
     */
    void addObject(Object* object);

    /**
     * @brief Add a new light to the scene.
     * 
     * @param light element to add.
     */
    void addLight(PointLight* light) { this->m_lights.pointLights.push_back(light); }
    void addLight(DirectionalLight* light) { this->m_lights.dirLights.push_back(light); }
    void addLight(SpotLight* light) { this->m_lights.spotLights.push_back(light); }

    /**
     * @brief Return the number of cameras in the scene.
     */
    const unsigned int camerasCount() const { return m_cameras.size(); }

    /**
     * @brief Return the number of lights in the scene.
     */
    const unsigned int lightsCount() const;

    /**
     * @brief Return the number of objects in the scene.
     */
    const unsigned int objectsCount() const { return m_objects.size(); }

    /**
     * @brief Return the camera with identifier = index
     * 
     * @param index index of the element to retrieve
     */
    Camera* getCamera(unsigned int index) const { return m_cameras.at(index); }

    /**
     * @brief Return the light with identifier = index
     * 
     * @param index index of the element to retrieve
     */
    // Light* getLight(unsigned int index) const { return m_lights.at(index); }

    /**
     * @brief Return the object with identifier = index
     * 
     * @param index index of the element to retrieve
     */
    Object* getObject(unsigned int index) const { return m_objects.at(index); }

    std::vector<Sphere*> getSpheresRT() const;

private:
    unsigned int m_activeCamera;
    ProjViewMatrix m_activeCameraPV;

    std::vector<Camera*> m_cameras;
    std::vector<Object*> m_objects;

    LightGroup m_lights;

};