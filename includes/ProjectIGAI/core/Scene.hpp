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
#include <ProjectIGAI/graphics/Object.hpp>

#define POINT_LIGHT_INDEX 0
#define DIR_LIGHT_INDEX 1
#define SPOT_LIGHT_INDEX 2

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
    void addCamera(Camera* camera) {
        this->m_cameras.push_back(camera);
        if(camerasCount() == 1) { this->m_activeCamera = 0; }
    }

    /**
     * @brief Add a new object to the scene.
     * 
     * @param object element to add.
     */
    void addNode(Node* node) { this->m_nodes.push_back(node); }

    /**
     * @brief Add a new point light to the scene.
     * 
     * @param light element to add.
     */
    void addLight(PointLight* light) { this->m_lights.pointLights.push_back(light); }

    /**
     * @brief Add a new directional light to the scene.
     * 
     * @param light element to add.
     */
    void addLight(DirectionalLight* light) { this->m_lights.dirLights.push_back(light); }

    /**
     * @brief Add a new spot light to the scene.
     * 
     * @param light element to add.
     */
    void addLight(SpotLight* light) { this->m_lights.spotLights.push_back(light); }

    /**
     * @brief Return the number of cameras in the scene.
     */
    const unsigned int camerasCount() const { return m_cameras.size(); }

    /**
     * @brief Return the number of lights in the scene for each type.
     * - x = point lights
     * - y = dir lights
     * - z = spot lights
     */
    const glm::vec3 lightsCount() const;

    /**
     * @brief Return the number of objects in the scene.
     */
    const unsigned int objectsCount() const { return m_nodes.size(); }

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
     * @warning DEPRECATED - UNUSED
     */
    // Light* getLight(unsigned int index) const { return m_lights.at(index); }

    /**
     * @brief Return the object with identifier = index
     * 
     * @param index index of the element to retrieve
     */
    Node* getNode(unsigned int index) const { return m_nodes.at(index); }

    /**
     * @brief Return the 'index' light of type 'type'
     * 
     * @param index index of the light in it's type
     * @param type type of the light
     * @return reference to the light selected, null if not exist;
     */
    Light* getLight(unsigned int index, unsigned int type) const;

    DirectionalLight* getMainDirectionalLight() { return m_lights.dirLights[0]; }

    /**
     * @brief Return all the scene objects
     * 
     * @return all scene objects
     */
    std::vector<Node*> getAllObjects() { return m_nodes; }

    /**
     * @brief Get a pointer on the scene background color for ImGui modification
     * 
     * @return pointer to the background color
     */
    float* getBackgroundColorPointer() { return &m_backgroundColor.x; }

    /**
     * @brief Get the background color.
     * 
     * @return background color
     */
    const glm::vec3 getBackgroundColor() const { return m_backgroundColor; }

    /**
     * @brief Set the background color.
     * 
     * @param color new background color
     */
    void setBackgroundColor(const glm::vec4 color) { m_backgroundColor = color; }


    std::string& skyboxName() { return m_skyboxName; }

    int& skyboxAttr() { return m_skybox; }

    bool skyboxActive() const { return m_skyboxActive; }

    void setSkyboxActive(const bool value) { m_skyboxActive = value; }

    int getRenderingMode() const { return m_renderingMode; }

    void setRenderingMode(const int mode) { m_renderingMode = mode; }

    void update(float dt);

private:

    /** Background color */
    glm::vec3 m_backgroundColor = {0.1f, 0.1f, 0.1f};

    /** ID of active camera */
    unsigned int m_activeCamera;

    /** Projection/view matrix of the active camera */
    ProjViewMatrix m_activeCameraPV;

    /** List of cameras in the scene */
    std::vector<Camera*> m_cameras;

    /** List of objects in the scene */
    std::vector<Node*> m_nodes;

    /** Tuple of lists of lights in the scene */
    LightGroup m_lights;

    /** Skybox */
    int m_skybox;
    bool m_skyboxActive = false;
    std::string m_skyboxName = "Bell-Tower";

    int m_renderingMode;
};