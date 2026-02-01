#include <ProjectIGAI/core/SceneParser.hpp>

#include <fstream>
#include <iostream>

#include <ProjectIGAI/graphics/Mesh.hpp>
#include <ProjectIGAI/graphics/Sphere.hpp>
#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/graphics/Light.hpp>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/geometry/BezierCurve.hpp>
#include <ProjectIGAI/geometry/BezierSurface.hpp>
#include <ProjectIGAI/core/utils.hpp>

std::unordered_map<SceneParser::ElementType, unsigned int> SceneParser::retrieveSceneCounts(const json& data)
{
    std::unordered_map<ElementType, unsigned int> sceneCounts;

    for(auto& item : data) {
        if(item["type"] == nullptr) {
            Logger::logError("Scene object must have a 'type' defined");
            exit(1);
        }

        auto it = s_TypeAliases.find(item["type"]);
        if(it == s_TypeAliases.end()) {
            Logger::logError("Type " + (std::string)item["type"] + " does not exist");
            exit(1);
        }

        ElementType type = it->second;
        (sceneCounts.find(type) == sceneCounts.end()) ? sceneCounts[type] = 1 : sceneCounts[type]++;
    }

    return sceneCounts;
}

Scene SceneParser::parseScene(const json& data)
{
    Scene newScene;

    for(auto& [objectName, item] : data.items()) {
        try {
            addObjectToScene(&newScene, item, objectName);
        }
        catch (const json::type_error& e) {
            const std::string errorMessage = string_format("[PARSING] Type error in object '%s'"
                "\nDetails : %s", objectName.c_str(), e.what());
            Logger::logError(errorMessage);
            exit(15);
        }
        catch (const json::out_of_range& e) {
            const std::string errorMessage = string_format("[PARSING] Value missing in object '%s'"
                "\nDetails : %s", objectName.c_str(), e.what());
            Logger::logError(errorMessage);
            exit(16);
        }
    }

    if(newScene.camerasCount() == 0)
        Logger::logWarning("Scene without camera will not display anything");

    return newScene;
}

void SceneParser::addObjectToScene(Scene *scene, json item, std::string name)
{
    if(item["type"] == nullptr) {
        Logger::logError("Scene object must have a 'type' defined");
        exit(1);
    }

    auto it = s_TypeAliases.find(item["type"]);
    if(it == s_TypeAliases.end()) {
        Logger::logError("Type " + (std::string)item["type"] + " does not exist");
        exit(1);
    }
    
    ElementType type = it->second;
    switch(type) {
    case CAMERA:
        parseObjectAs_Camera(scene, item, name);
        break;

    case POINT_LIGHT:
        parseObjectAs_PointLight(scene, item, name);
        break;

    case SPOT_LIGHT:
        parseObjectAs_SpotLight(scene, item, name);
        break;

    case DIR_LIGHT:
        parseObjectAs_DirectionalLight(scene, item, name);
        break;
        
    case SPHERE:
        parseObjectAs_Sphere(scene, item, name);
        break;

    case MESH:
        parseObjectAs_Mesh(scene, item, name);
        break;

    case BEZIER_CURVE:
        parseObjectAs_BezierCurve(scene, item, name);
        break;
    
    case BEZIER_SURFACE:
        parseObjectAs_BezierSurface(scene, item, name);
        break;
    }
}

void SceneParser::parseObjectAs_Camera(Scene *scene, json item, std::string name)
{
    /* Roll(x), Pitch(y), Yaw(z)
     * Roll is fixed (no camera roll), up vector is (0, 1, 0)
     */
    Transform transform = jsonToTransform(item["transform"]);
    scene->addCamera(new Camera(
        transform.position,
        glm::vec3(0.f, 1.f, 0.f),
        transform.rotation.z,
        transform.rotation.y
    ));
}

void SceneParser::parseObjectAs_Mesh(Scene *scene, json item, std::string name)
{
    // Base class construction
    std::string file = item["file"];
    Mesh* mesh = new Mesh(file);

    // Global object configuration
    configureObject(mesh, item);

    // Adding to scene
    scene->addObject(mesh);
}

void SceneParser::parseObjectAs_Sphere(Scene *scene, json item, std::string name)
{
    // Base class construction
    float size = jsonToFloat(item, "size");
    Sphere* sphere = new Sphere(size);

    // Global object configuration
    configureObject(sphere, item);

    // Specific class configuration
    if(item["ray-tracing.type"] != nullptr)
        sphere->Type = Hittable::HitTypeCatalog.at(item["ray-tracing.type"]);

    // Adding to scene
    scene->addObject(sphere);
}

void SceneParser::parseObjectAs_BezierCurve(Scene *scene, json item, std::string name)
{
    // Base class construction
    vec3Array controlPoints = jsonToVec3Array(item, "control points");
    BezierCurve* bezierCurve = new BezierCurve(controlPoints);

    // Global object configuration
    configureObject(bezierCurve, item);

    // Adding to scene
    scene->addObject(bezierCurve);
}

void SceneParser::parseObjectAs_BezierSurface(Scene *scene, json item, std::string name)
{
    // Base class construction
    vec3Grid controlPoints = jsonToVec3Grid(item, "control points");
    BezierSurface* bezierSurface = new BezierSurface(controlPoints);

    // Global object configuration
    configureObject(bezierSurface, item);

    // Adding to scene
    scene->addObject(bezierSurface);
}

void SceneParser::parseObjectAs_PointLight(Scene *scene, json item, std::string name)
{
    // Base class construction
    PointLight* pointLight = new PointLight();

    // Global object configuration
    configureLight(pointLight, item);

    // Specific class configuration
    if(item["radius"] != nullptr) pointLight->setRadius(jsonToFloat(item, "radius"));

    // Adding to scene
    scene->addLight(pointLight);
}

void SceneParser::parseObjectAs_DirectionalLight(Scene *scene, json item, std::string name)
{
    // Base class construction
    DirectionalLight* dirLight = new DirectionalLight();

    // Global object configuration
    configureLight(dirLight, item);

    // Adding to scene
    scene->addLight(dirLight);
}

void SceneParser::parseObjectAs_SpotLight(Scene *scene, json item, std::string name)
{
    // Base class construction
    SpotLight* spotLight = new SpotLight();

    // Global object configuration
    configureLight(spotLight, item);

    // Specific class configuration
    if(item["radius"]      != nullptr) spotLight->setRadius(jsonToFloat(item, "radius"));
    if(item["cutOff"]      != nullptr) spotLight->setCutOff(glm::cos(glm::radians(jsonToFloat(item, "cutOff"))));
    if(item["outerCutOff"] != nullptr) spotLight->setOuterCutOff(glm::cos(glm::radians(jsonToFloat(item, "outerCutOff"))));

    // Adding to scene
    scene->addLight(spotLight);
}

void SceneParser::configureObject(Object* object, json item)
{
    if(item["transform"] != nullptr) object->setTransform(jsonToTransform(item["transform"]));
    if(item["material"]  != nullptr) object->setMaterial(jsonToMaterial(item["material"]));
    if(item["texture"]   != nullptr) object->addTexture(item["texture"]);
}

void SceneParser::configureLight(Light* light, json item)
{
    if(item["transform"] != nullptr) light->setTransform(jsonToTransform(item["transform"]));
    if(item["material"]  != nullptr) light->setLightMaterial(jsonToLightProperties(item["material"]));
}

glm::vec3 SceneParser::jsonToVec3(json json, const std::string &attribute)
{
    std::vector<float> value = static_cast<std::vector<float>>(json[attribute]);
    return glm::vec3(value[0], value[1], value[2]);
}

float SceneParser::jsonToFloat(json json, const std::string &attribute)
{
    float value = static_cast<float>(json[attribute]);
    return value;
}

Transform SceneParser::jsonToTransform(json json)
{
    return {
        jsonToVec3(json, "position"),
        jsonToVec3(json, "scale"),
        jsonToVec3(json, "rotation")
    };
}

Material SceneParser::jsonToMaterial(json json)
{
    return {
        ShaderManager::getInstance().getShader(json["shader"]),
        jsonToShaderMaterial(json["shader material"])
    };
}

ShaderMaterial SceneParser::jsonToShaderMaterial(json json)
{
    return {
        jsonToVec3(json, "color"),
        jsonToFloat(json, "roughness"),
        jsonToFloat(json, "metallic")
    };
}

LightProperties SceneParser::jsonToLightProperties(json json)
{
    return {
        jsonToVec3(json, "color"),
        jsonToFloat(json, "intensity")
    };
}

vec3Array SceneParser::jsonToVec3Array(json json, const std::string &attribute)
{
    vec3Array value;
    for(const auto& item : json[attribute]) {
        value.push_back({item[0].get<float>(), item[1].get<float>(), item[2].get<float>()});
    }

    return value;
}

vec3Grid SceneParser::jsonToVec3Grid(json json, const std::string &attribute)
{
    vec3Grid value;
    unsigned int i = 0;

    for(const auto& sub : json[attribute]) {
        value.push_back(vec3Array());
        for(const auto& item : sub) {
            value[i].push_back({item[0].get<float>(), item[1].get<float>(), item[2].get<float>()});
        }
        ++i;
    }

    return value;
}
