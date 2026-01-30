#pragma once

#include <extern/json.hpp>
#include <ProjectIGAI/core/Scene.hpp>

using json = nlohmann::json;
using vec3Array = std::vector<glm::vec3>;
using vec3Grid = std::vector<std::vector<glm::vec3>>;

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
        SPHERE,
        MESH,
        BEZIER_CURVE, BEZIER_SURFACE
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
        {"mesh", MESH},
        {"bezier curve", BEZIER_CURVE},
        {"bezier surface", BEZIER_SURFACE}
    };

    /**
     * @brief Read the file only to retrieve object counts. This function allow to
     * construct shaders based on the scene we are going to read.
     * 
     * @param file path to the JSON scene file
     * @return count of each object stored in a map
     */
    static std::unordered_map<ElementType, unsigned int> retrieveSceneCounts(const std::string& file);

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
     * @brief Parse the object as a directional light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_DirectionalLight(Scene* scene, json item);

    /**
     * @brief Parse the object as a spot light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_SpotLight(Scene* scene, json item);

    /**
     * @brief Parse the object as a bezier curve.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_BezierCurve(Scene* scene, json item);

    /**
     * @brief Parse the object as a bezier surface.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_BezierSurface(Scene* scene, json item);

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
     */
    static Transform jsonToTransform(json json);

    /**
     * @brief Auxiliary function for casting json element to Material.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static Material jsonToMaterial(json json);

    /**
     * @brief Auxiliary function for casting json element to ShaderMaterial.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static ShaderMaterial jsonToShaderMaterial(json json);

    /**
     * @brief Auxiliary function for casting json element to LightMaterial.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static LightProperties jsonToLightMaterial(json json);

    /**
     * @brief Auxiliary function for casting json element to Vec3Array.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static vec3Array jsonToVec3Array(json json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to Vec3Grid.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static vec3Grid jsonToVec3Grid(json json, const std::string& attribute);
};