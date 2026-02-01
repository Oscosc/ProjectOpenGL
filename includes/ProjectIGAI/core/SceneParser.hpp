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
    static std::unordered_map<ElementType, unsigned int> retrieveSceneCounts(const json& data);

    /**
     * @brief Main function for parsing a Scene file (.json)
     * 
     * @param file path to the JSON scene file
     */
    static Scene parseScene(const json& data);

    /**
     * @brief Global function for adding a new element (all types) in the scene.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void addObjectToScene(Scene* scene, const json& item, std::string name);

private:

    /**
     * @brief Parse the object as a camera.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_Camera(Scene* scene, const json& item, std::string name);

    /**
     * @brief Parse the object as a mesh.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_Mesh(Scene* scene, const json& item, std::string name);

    /**
     * @brief Parse the object as a sphere.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_Sphere(Scene* scene, const json& item, std::string name);

        /**
     * @brief Parse the object as a bezier curve.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_BezierCurve(Scene* scene, const json& item, std::string name);

    /**
     * @brief Parse the object as a bezier surface.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_BezierSurface(Scene* scene, const json& item, std::string name);

    /**
     * @brief Parse the object as a point light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_PointLight(Scene* scene, const json& item, std::string name);

    /**
     * @brief Parse the object as a directional light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_DirectionalLight(Scene* scene, const json& item, std::string name);

    /**
     * @brief Parse the object as a spot light.
     * 
     * @param scene Scene where element will be added
     * @param item JSON sub-object that contain the element informations.
     */
    static void parseObjectAs_SpotLight(Scene* scene, const json& item, std::string name);

    /**
     * @brief 
     * 
     * @param object 
     * @param item 
     */
    static void configureObject(Object* object, const json& item, const std::string name);

    /**
     * @brief 
     * 
     * @param light 
     * @param item 
     */
    static void configureLight(Light* light, const json& item, const std::string name);

    /**
     * @brief 
     * 
     * @param node 
     * @param item 
     * @param name 
     */
    static void configureNode(Node* node, const json& item, const std::string name);

    /**
     * @brief Auxiliary function for casting json element to glm::vec3.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static glm::vec3 jsonToVec3(const json& json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to float.
     * 
     * @param json JSON hierarchy containing the attribute
     * @param attribute name of the attribute to convert
     */
    static float jsonToFloat(const json& json, const std::string& attribute);

    /**
     * @brief Auxiliary function for casting json element to Transform.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static Transform jsonToTransform(const json& json);

    /**
     * @brief Auxiliary function for casting json element to Material.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static Material jsonToMaterial(const json& json);

    /**
     * @brief Auxiliary function for casting json element to ShaderMaterial.
     * 
     * @param json JSON hierarchy containing the attribute
     */
    static ShaderMaterial jsonToShaderMaterial(const json& json);

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