#pragma once

#include "../includes/json.hpp"
#include "Scene.hpp"

using json = nlohmann::json;

struct Transform;
struct Material;

/**
 * @brief Class containing parsing functions for scene files (.json)
 */
class SceneParser
{
public:

    /**
     * @brief List of all elements type that can be interpreted in a JSON file.
     * Undeclared elements will call an error.
     */
    enum ElementType {
        CAMERA,
        POINT_LIGHT, SPOT_LIGHT, DIR_LIGHT,
        SPHERE,
        MESH
    };

    /**
     * @brief List of strings associated with Elements types.
     */
    static inline const std::unordered_map<std::string, ElementType> s_TypeAliases = {
        {"camera", CAMERA},
        {"point light", POINT_LIGHT},
        {"spot light", SPOT_LIGHT},
        {"dir light", DIR_LIGHT},
        {"sphere", SPHERE},
        {"mesh", MESH}
    };

    /**
     * @brief Main function for parsing a Scene file (.json)
     * 
     * @param file path to the JSON scene file
     */
    static Scene parseScene(const std::string& file);

    /**
     * @brief Global function for adding a new element (all types) in the scene.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void addObjectToScene(Scene* scene, json item);

private:

    /**
     * @brief Parse the object as a camera.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_Camera(Scene* scene, json item);

    /**
     * @brief Parse the object as a mesh.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_Mesh(Scene* scene, json item);

    /**
     * @brief Parse the object as a sphere.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_Sphere(Scene* scene, json item);

    /**
     * @brief Parse the object as a point light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_PointLight(Scene* scene, json item);

    /**
     * @brief Auxiliary function for casting json element to glm::vec3.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static glm::vec3 jsonToVec3(json json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to float.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static float jsonToFloat(json json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to Transform.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static Transform jsonToTransform(json json);

    /**
     * @brief Auxiliary function for casting json element to Material.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static Material jsonToMaterial(json json);
};