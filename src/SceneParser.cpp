#include "SceneParser.hpp"

#include <fstream>
#include <iostream>

#include "Mesh.hpp"
#include "Sphere.hpp"

Scene SceneParser::parseScene(const std::string &file)
{
    std::ifstream stream(file);
    json data = json::parse(stream);

    Scene newScene;

    for(auto& item : data) {
        addObjectToScene(&newScene, item);
    }

    if(newScene.camerasCount() == 0)
        std::cout << "[WARNING] Scene without camera will not display anything" << std::endl;

    return newScene;
}

void SceneParser::addObjectToScene(Scene *scene, json item)
{
    if(item["type"] == nullptr) {
        std::cout << "[ERROR] Scene object must have a 'type' defined" << std::endl;
        exit(1);
    }

    auto it = s_TypeAliases.find(item["type"]);
    if(it == s_TypeAliases.end()) {
        std::cout << "[ERROR] Type " << item["type"] << " does not exist" << std::endl;
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
        std::cout << "[ERROR] 'spot_light' Not implemented yet" << std::endl;
        exit(1);

    case DIR_LIGHT:
        std::cout << "[ERROR] 'dir_light' Not implemented yet" << std::endl;
        exit(1);
        
    case SPHERE:
        parseObjectAs_Sphere(scene, item);
        break;

    case MESH:
        parseObjectAs_Mesh(scene, item);
        break;
    }
}

void SceneParser::parseObjectAs_Camera(Scene *scene, json item)
{
    scene->addCamera(new Camera(jsonToVec3(item, "position")));
}

void SceneParser::parseObjectAs_Mesh(Scene *scene, json item)
{
    glm::vec3 position = jsonToVec3(item, "position");
    glm::vec3 rotation = jsonToVec3(item, "rotation");
    glm::vec3 scale = jsonToVec3(item, "scale");

    scene->addObject(new Mesh(item["file"], {position, scale, rotation}));

    Mesh* obj = static_cast<Mesh*>(scene->getObject(scene->objectsCount() - 1));
    obj->displayInformations();
}

void SceneParser::parseObjectAs_Sphere(Scene *scene, json item)
{
    glm::vec3 position = jsonToVec3(item, "position");
    float size = jsonToFloat(item, "size");

    scene->addObject(new Sphere(size, position));
}

void SceneParser::parseObjectAs_PointLight(Scene *scene, json item)
{
    glm::vec3 color = jsonToVec3(item, "color");
    glm::vec3 position = jsonToVec3(item, "position");
    float strength = jsonToFloat(item, "strength");

    scene->addLight(new PointLight(position, color, strength));
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
