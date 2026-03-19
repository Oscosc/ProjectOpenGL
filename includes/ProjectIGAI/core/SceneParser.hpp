#pragma once

#include <extern/json.hpp>
#include <ProjectIGAI/core/Scene.hpp>

using json = nlohmann::json;
using vec3Array = std::vector<glm::vec3>;
using vec3Grid = std::vector<vec3Array>;

struct Transform;
struct Material;
struct ShaderMaterial;
struct LightProperties;

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
        OBJECT
    };

    /**
     * @brief List of strings associated with Elements types.
     */
    static inline const std::unordered_map<std::string, ElementType> s_TypeAliases = {
        {"camera", CAMERA},
        {"point_light", POINT_LIGHT},
        {"spot_light", SPOT_LIGHT},
        {"directional_light", DIR_LIGHT},
        {"object", OBJECT},
        {"model", OBJECT}
    };

    /**
     * @brief Read the file only to retrieve object counts. This function allow to
     * construct shaders based on the scene we are going to read.
     * 
     * @param file path to the JSON scene file
     * @return count of each object stored in a map
     */
    static std::unordered_map<ElementType, unsigned int> retrieveSceneCounts(const json& data);

    static void parseScene(Scene* scene, const std::string &filePath);

private:

    /**
     * @brief Parse the object as a camera.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static Camera* parseCamera(const json& node);

    /**
     * @brief Parse the object as a mesh.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static Object* parseObject(const json& node);

    /**
     * @brief Parse the object as a point light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static PointLight* parsePointLight(const json& node);

    /**
     * @brief Parse the object as a directional light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static DirectionalLight* parseDirectionalLight(const json& node);

    /**
     * @brief Parse the object as a spot light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static SpotLight* parseSpotLight(const json& node);

    /**
     * @brief Auxiliary function for casting json element to glm::vec3.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static glm::vec3 jsonToVec3(const json& json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to Transform.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static Transform jsonToTransform(const json& json);

    /**
     * @brief Auxiliary function for casting json element to LightMaterial.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static LightProperties jsonToLightProperties(const json& json);

    /**
     * @brief Auxiliary function for casting json element to Vec3Array.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static vec3Array jsonToVec3Array(const json& json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to Vec3Grid.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static vec3Grid jsonToVec3Grid(const json& json, const std::string &attribute);
};