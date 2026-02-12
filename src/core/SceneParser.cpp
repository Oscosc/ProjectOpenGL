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
        if(item.at("type") == nullptr) {
            Logger::logError("Scene object must have a 'type' defined");
            exit(1);
        }

        auto it = s_TypeAliases.find(item.at("type"));
        if(it == s_TypeAliases.end()) {
            Logger::logError("Type " + (std::string)item.at("type") + " does not exist");
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

void SceneParser::addObjectToScene(Scene *scene, const json& item, std::string name)
{
    if(item.at("type") == nullptr) {
        Logger::logError("Scene object must have a 'type' defined");
        exit(1);
    }

    auto it = s_TypeAliases.find(item.at("type"));
    if(it == s_TypeAliases.end()) {
        Logger::logError("Type " + (std::string)item.at("type") + " does not exist");
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

void SceneParser::parseObjectAs_Camera(Scene *scene, const json& item, std::string name)
{
    /* Roll(x), Pitch(y), Yaw(z)
     * Roll is fixed (no camera roll), up vector is (0, 1, 0)
     */
    Transform transform = jsonToTransform(item.at("transform"));
    scene->addCamera(new Camera(
        transform.position,
        glm::vec3(0.f, 1.f, 0.f),
        transform.rotation.z,
        transform.rotation.y
    ));
}

void SceneParser::parseObjectAs_Mesh(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    std::string file = item.at("file");
    Mesh* mesh = new Mesh(file);

    // Global object configuration
    configureObject(mesh, item, name);

    // Adding to scene
    scene->addObject(mesh);
}

void SceneParser::parseObjectAs_Sphere(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    float size = jsonToFloat(item, "size");
    Sphere* sphere = new Sphere(size);

    // Global object configuration
    configureObject(sphere, item, name);

    // Specific class configuration
    if(item.contains("ray-tracing.type"))
        sphere->Type = Hittable::HitTypeCatalog.at(item.at("ray-tracing.type"));

    // Adding to scene
    scene->addObject(sphere);
}

void SceneParser::parseObjectAs_BezierCurve(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    vec3Array controlPoints = jsonToVec3Array(item, "control points");
    BezierCurve* bezierCurve = new BezierCurve(controlPoints);

    // Global object configuration
    configureObject(bezierCurve, item, name);

    // Adding to scene
    scene->addObject(bezierCurve);
}

void SceneParser::parseObjectAs_BezierSurface(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    vec3Grid controlPoints = jsonToVec3Grid(item, "control points");
    BezierSurface* bezierSurface = new BezierSurface(controlPoints);

    // Global object configuration
    configureObject(bezierSurface, item, name);

    // Adding to scene
    scene->addObject(bezierSurface);
}

void SceneParser::parseObjectAs_PointLight(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    PointLight* pointLight = new PointLight();

    // Global object configuration
    configureLight(pointLight, item, name);

    // Specific class configuration
    if(item.contains("radius")) pointLight->setRadius(jsonToFloat(item, "radius"));

    // Adding to scene
    scene->addLight(pointLight);
}

void SceneParser::parseObjectAs_DirectionalLight(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    DirectionalLight* dirLight = new DirectionalLight();

    // Global object configuration
    configureLight(dirLight, item, name);

    // Adding to scene
    scene->addLight(dirLight);
}

void SceneParser::parseObjectAs_SpotLight(Scene *scene, const json& item, std::string name)
{
    // Base class construction
    SpotLight* spotLight = new SpotLight();

    // Global object configuration
    configureLight(spotLight, item, name);

    // Specific class configuration
    if(item.contains("radius"))      spotLight->setRadius(jsonToFloat(item, "radius"));
    if(item.contains("cutOff"))      spotLight->setCutOff(glm::cos(glm::radians(jsonToFloat(item, "cutOff"))));
    if(item.contains("outerCutOff")) spotLight->setOuterCutOff(glm::cos(glm::radians(jsonToFloat(item, "outerCutOff"))));

    // Adding to scene
    scene->addLight(spotLight);
}

void SceneParser::configureObject(Object* object, const json& item, const std::string name)
{
    configureNode(object, item, name);
    if(item.contains("material")) object->setMaterial(jsonToMaterial(item.at("material")));
    if(item.contains("texture")) object->addTexture(item.at("texture"));
}

void SceneParser::configureLight(Light* light, const json& item, const std::string name)
{
    configureNode(light, item, name);
    if(item.contains("material")) light->setLightMaterial(jsonToLightProperties(item.at("material")));
}

void SceneParser::configureNode(Node* node, const json& item, const std::string name)
{
       if(item.contains("transform")) node->setTransform(jsonToTransform(item.at("transform")));
       node->setName(name);
}

glm::vec3 SceneParser::jsonToVec3(const json& json, const std::string &attribute)
{
    return glm::vec3(
        json.at(attribute)[0].get<float>(),
        json.at(attribute)[1].get<float>(),
        json.at(attribute)[2].get<float>()
    );
}

float SceneParser::jsonToFloat(const json& json, const std::string &attribute)
{
    return json.at(attribute).get<float>();
}

Transform SceneParser::jsonToTransform(const json& json)
{
    return {
        jsonToVec3(json, "position"),
        jsonToVec3(json, "scale"),
        jsonToVec3(json, "rotation")
    };
}

Material SceneParser::jsonToMaterial(const json& json)
{
    return {
        ShaderManager::getInstance().getResource(json.at("shader")),
        jsonToShaderMaterial(json.at("shader material"))
    };
}

ShaderMaterial SceneParser::jsonToShaderMaterial(const json& json)
{
    return {
        jsonToVec3(json, "color"),
        jsonToFloat(json, "roughness"),
        jsonToFloat(json, "metallic")
    };
}

LightProperties SceneParser::jsonToLightProperties(const json& json)
{
    return {
        jsonToVec3(json, "color"),
        jsonToFloat(json, "intensity")
    };
}

vec3Array SceneParser::jsonToVec3Array(const json& json, const std::string &attribute)
{
    vec3Array value;
    for(const auto& item : json[attribute]) {
        value.push_back({item[0].get<float>(), item[1].get<float>(), item[2].get<float>()});
    }

    return value;
}

vec3Grid SceneParser::jsonToVec3Grid(const json& json, const std::string &attribute)
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
