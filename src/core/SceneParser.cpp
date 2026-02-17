#include <ProjectIGAI/core/SceneParser.hpp>

#include <fstream>
#include <iostream>

#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/graphics/Light.hpp>
#include <ProjectIGAI/graphics/GeometryManager.hpp>
#include <ProjectIGAI/graphics/TextureManager.hpp>
#include <ProjectIGAI/graphics/MaterialManager.hpp>
#include <ProjectIGAI/graphics/StandardPBRMaterial.hpp>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/core/utils.hpp>

std::unordered_map<SceneParser::ElementType, unsigned int> SceneParser::retrieveSceneCounts(const json& data)
{
    std::unordered_map<ElementType, unsigned int> sceneCounts;

    const json& dataToIterate = data.contains("nodes") ? data["nodes"] : data;

    for(auto& item : dataToIterate) {
        if (!item.is_object()) continue;

        if(!item.contains("type")) {
            Logger::logError("Scene object must have a 'type' defined");
            continue;
        }

        std::string typeStr = item["type"];
        auto it = s_TypeAliases.find(typeStr);
        
        if(it == s_TypeAliases.end()) {
            Logger::logWarning("Type " + typeStr + " not found in aliases, skipping count.");
            continue;
        }

        ElementType type = it->second;
        sceneCounts[type]++;
    }

    return sceneCounts;
}

void SceneParser::parseScene(Scene* scene, const std::string &filePath)
{
    std::ifstream stream(filePath);
    const json jFile = json::parse(stream);

    for (auto& node : jFile["nodes"]) {
        try {
            std::string type = node.value("type", "object");
            Logger::logPerf("Trying to load node '" + node.value("name", "unnamed") + "' of type '" + type + "'.");

            Node* newNode = nullptr;

            if (type == "object")
                newNode = parseObject(node);
            else if (type == "directional_light")
                newNode = parseDirectionalLight(node);
            else if (type == "point_light")
                newNode = parsePointLight(node);
            else if (type == "spot_light")
                newNode = parseSpotLight(node);
            else if (type == "camera")
                newNode = parseCamera(node);
            
            if(newNode && node.contains("transform"))
                newNode->setTransform(jsonToTransform(node["transform"]));

            
            if (!newNode) return;
    
            if (auto* light = dynamic_cast<PointLight*>(newNode)) {
                scene->addLight(light);
            }

            else if (auto* light = dynamic_cast<DirectionalLight*>(newNode)) {
                scene->addLight(light);
            }

            else if (auto* light = dynamic_cast<SpotLight*>(newNode)) {
                scene->addLight(light);
            }

            else if (auto* cam = dynamic_cast<Camera*>(newNode)) {
                scene->addCamera(cam);
            }

            else if (auto* obj = dynamic_cast<Object*>(newNode)) {
                scene->addObject(obj);
            }
            
            //if(newNode) scene->addNode(newNode);

        } catch (const json::exception& e) {
            Logger::logError("JSON Logic Error (ID " + std::to_string(e.id) + "): " + e.what());
            Logger::logError("On node: " + node.dump());
        }
    }

    if(!jFile.contains("camera")) return;

    for (auto& camera : jFile["camera"]) {
        scene->addCamera(parseCamera(camera));
    }
}

Camera* SceneParser::parseCamera(const json& node)
{
    /* Roll(x), Pitch(y), Yaw(z)
     * Roll is fixed (no camera roll), up vector is (0, 1, 0)
     */
    Transform transform = jsonToTransform(node.at("transform"));
    return new Camera(
        transform.position,
        glm::vec3(0.f, 1.f, 0.f),
        transform.rotation.z,
        transform.rotation.y
    );
}

Object* SceneParser::parseObject(const json& node)
{
    Object* obj = new Object(node.value("name", "Object"));

    if(node.contains("geometry"))
    {
        json data = node["geometry"];
        std::string type = data.value("type", "");

        Geometry* geometry = nullptr;

        if (type == "sphere") {
            float r = data.value("radius", 1.0f);
            int seg = data.value("segments", 32);

            geometry = GeometryManager::getInstance().getSphere(r, seg);
        }
        else if (type == "mesh") {
            std::string path = data.value("path", "");

            geometry = GeometryManager::getInstance().getMesh(path);
        }
        else if (type == "cube") {
            // TODO
        }
        else if (type == "bezier curve") {
            // TODO
        }
        else if (type == "bezier surface") {
            // TODO
        }

        obj->setGeometry(geometry);
    }

    if(node.contains("material"))
    {
        bool created;
        json data = node["material"];
        StandardPBRMaterial* mat = MaterialManager::getInstance().create<StandardPBRMaterial>(node["name"], created);
        
        if(!created) {
            obj->setMaterial(mat);
            return obj;
        }

        if(data.contains("albedo")) {
            auto a = data["albedo"];
            mat->albedo = glm::vec3(a[0], a[1], a[2]);
        }
        mat->roughness = data.value("roughness", 0.5f);
        mat->metallic  = data.value("metallic", 0.0f);

        if(data.contains("textures")) {
            json texData = data["textures"];

            if(texData.contains("albedo")) {
                std::string path = texData["albedo"];
                mat->albedoMap = TextureManager::getInstance().loadTexture(path);
            }

            if(texData.contains("roughness")) {
                std::string path = texData["roughness"];
                mat->roughnessMap = TextureManager::getInstance().loadTexture(path);
            }

            if(texData.contains("metallic")) {
                std::string path = texData["metallic"];
                mat->metallicMap = TextureManager::getInstance().loadTexture(path);
            }
        }

        obj->setMaterial(mat);
    }

    return obj;
}

PointLight* SceneParser::parsePointLight(const json &node)
{
    PointLight* light = new PointLight();

    light->setName(node.value("name", "Point light"));
    light->setLightMaterial(jsonToLightProperties(node));
    light->setRadius(node.value("radius", 1.0f));

    return light;
}

DirectionalLight* SceneParser::parseDirectionalLight(const json &node)
{
    DirectionalLight* light = new DirectionalLight();
    light->setName(node.value("name", "Point light"));
    light->setLightMaterial(jsonToLightProperties(node));

    return light;
}

SpotLight* SceneParser::parseSpotLight(const json &node)
{
    SpotLight* light = new SpotLight();

    light->setName(node.value("name", "Point light"));
    light->setLightMaterial(jsonToLightProperties(node));
    light->setRadius(node.value("radius", 1.0f));
    light->setCutOff(node.value("cutOff", 0.0f));
    light->setOuterCutOff(node.value("outerCutOff", 1.0f));

    return light;
}

glm::vec3 SceneParser::jsonToVec3(const json& json, const std::string &attribute)
{
    return glm::vec3(
        json[attribute][0].get<float>(),
        json[attribute][1].get<float>(),
        json[attribute][2].get<float>()
    );
}

Transform SceneParser::jsonToTransform(const json& json)
{
    return {
        jsonToVec3(json, "position"),
        jsonToVec3(json, "scale"),
        jsonToVec3(json, "rotation")
    };
}

LightProperties SceneParser::jsonToLightProperties(const json& json)
{
    return {
        jsonToVec3(json, "color"),
        json["intensity"]
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
