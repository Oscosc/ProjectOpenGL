#include "SceneParser.hpp"

#include <fstream>
#include <iostream>

#include "Mesh.hpp"
#include "Sphere.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "Logger.hpp"
#include "BezierCurve.hpp"

Scene SceneParser::parseScene(const std::string &file)
{
    std::ifstream stream(file);
    json data = json::parse(stream);

    Scene newScene;

    for(auto& item : data) {
        addObjectToScene(&newScene, item);
    }

    if(newScene.camerasCount() == 0)
        Logger::logWarning("Scene without camera will not display anything");

    return newScene;
}

void SceneParser::addObjectToScene(Scene *scene, json item)
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
        parseObjectAs_Camera(scene, item);
        break;

    case POINT_LIGHT:
        parseObjectAs_PointLight(scene, item);
        break;

    case SPOT_LIGHT:
        Logger::logError("'spot_light' Not implemented yet");
        exit(1);

    case DIR_LIGHT:
        Logger::logError("'dir_light' Not implemented yet");
        exit(1);
        
    case SPHERE:
        parseObjectAs_Sphere(scene, item);
        break;

    case MESH:
        parseObjectAs_Mesh(scene, item);
        break;

    case BEZIER:
        parseObjectAs_BezierCurve(scene, item);
        break;
    }
}

void SceneParser::parseObjectAs_Camera(Scene *scene, json item)
{
    scene->addCamera(new Camera(jsonToVec3(item, "position")));
}

void SceneParser::parseObjectAs_Mesh(Scene *scene, json item)
{
    if(item["transform"] != nullptr) {
        if(item["material"] != nullptr) {
            scene->addObject(new Mesh(item["file"], jsonToTransform(item["transform"]), jsonToMaterial(item["material"])));
        } else {
            scene->addObject(new Mesh(item["file"], jsonToTransform(item["transform"])));
        }
    } else {
        scene->addObject(new Mesh(item["file"]));
    }
}

void SceneParser::parseObjectAs_Sphere(Scene *scene, json item)
{
    float size = jsonToFloat(item, "size");
    if(item["transform"] != nullptr) {
        if(item["material"] != nullptr) {
            scene->addObject(new Sphere(size, jsonToTransform(item["transform"]), jsonToMaterial(item["material"])));
            if(item["ray-tracing.type"] != nullptr) {
                Sphere* last = dynamic_cast<Sphere*>(scene->getObject(scene->objectsCount() - 1));
                last->Type = Hittable::HitTypeCatalog.at(item["ray-tracing.type"]);
            }
        } else {
            scene->addObject(new Sphere(size, jsonToTransform(item["transform"])));
        }
    } else {
        scene->addObject(new Sphere(size));
    }
}

void SceneParser::parseObjectAs_PointLight(Scene *scene, json item)
{
    glm::vec3 position = jsonToVec3(item, "position");
    if(item["material"] != nullptr) {
        scene->addLight(new PointLight(position, jsonToLightMaterial(item["material"])));
    } else {
        scene->addLight(new PointLight(position));
    }
}

void SceneParser::parseObjectAs_BezierCurve(Scene *scene, json item)
{
    vec3Array controlPoints = jsonToVec3Array(item, "control points");

    if(item["transform"] != nullptr) {
        if(item["material"] != nullptr) {
            scene->addObject(new BezierCurve(controlPoints, jsonToTransform(item["transform"]), jsonToMaterial(item["material"])));
        } else {
            scene->addObject(new BezierCurve(controlPoints, jsonToTransform(item["transform"])));
        }
    } else {
        scene->addObject(new BezierCurve(controlPoints));
    }
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
        jsonToVec3(json, "ambient"),
        jsonToVec3(json, "diffuse"),
        jsonToVec3(json, "specular"),
        jsonToFloat(json, "shininess")
    };
}

LightMaterial SceneParser::jsonToLightMaterial(json json)
{
    return {
        jsonToVec3(json, "ambient"),
        jsonToVec3(json, "diffuse"),
        jsonToVec3(json, "specular")
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
