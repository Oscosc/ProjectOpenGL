#include "Parser.hpp"

#include <fstream>
#include <iostream>

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
        break;

    case SPOT_LIGHT:
        std::cout << "[ERROR] 'spot_light' Not implemented yet" << std::endl;
        exit(1);

    case DIR_LIGHT:
        std::cout << "[ERROR] 'dir_light' Not implemented yet" << std::endl;
        exit(1);
        
    case SPHERE:
        break;

    case MESH:
        break;
    }
}

void SceneParser::parseObjectAs_Camera(Scene *scene, json item)
{
    std::vector<float> position = static_cast<std::vector<float>>(item["position"]);
    scene->addCamera(new Camera({position[0], position[1], position[2]}));
}

void SceneParser::parseObjectAs_Mesh(Scene *scene, json item)
{
}

void SceneParser::parseObjectAs_PointLight(Scene *scene, json item)
{
}
