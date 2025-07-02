#pragma once

#include "../includes/json.hpp"
#include "Scene.hpp"

using json = nlohmann::json;

struct Transform;
struct Material;

class SceneParser
{
public:
    enum ElementType {
        CAMERA,
        POINT_LIGHT, SPOT_LIGHT, DIR_LIGHT,
        SPHERE,
        MESH
    };
    static inline const std::unordered_map<std::string, ElementType> s_TypeAliases = {
        {"camera", CAMERA},
        {"point light", POINT_LIGHT},
        {"spot light", SPOT_LIGHT},
        {"dir light", DIR_LIGHT},
        {"sphere", SPHERE},
        {"mesh", MESH}
    };

    static Scene parseScene(const std::string& file);
    static void addObjectToScene(Scene* scene, json item);

private:
    static void parseObjectAs_Camera(Scene* scene, json item);
    static void parseObjectAs_Mesh(Scene* scene, json item);
    static void parseObjectAs_Sphere(Scene* scene, json item);
    static void parseObjectAs_PointLight(Scene* scene, json item);

    static glm::vec3 jsonToVec3(json json, const std::string& attribute);
    static float jsonToFloat(json json, const std::string& attribute);
    static Transform jsonToTransform(json json);
    static Material jsonToMaterial(json json);
};