#include "SceneParser.hpp"
#include "Scene.hpp"

Scene *SceneParser::parseScene(const std::string &file)
{
    std::string lineBuffer;
    std::ifstream reader(file);

    Scene newScene;

    unsigned int lineID = 1;

    while(std::getline(reader, lineBuffer)) {
        std::vector<std::string> tokens = split(lineBuffer, STD_SCENE_DELIMITER);
        KeywordInfo lineInfo;

        // Validating grammar
        if(!validateLine(tokens, lineInfo)) {
            std::cout << "[ERROR] " << file << ": Problem in file structure at line " << lineID << std::endl;
            exit(1);
        }

        // Creating object
        if(lineInfo.isObject) {
            switch(lineInfo.key) {
                case CAMERA: createCamera(tokens, &newScene, reader, lineBuffer);
                case POINT_LIGHT:
                case DIR_LIGHT:
                case SPOT_LIGHT:
                case SPHERE:
                case MESH:
            }
        }

        lineID++;
    } reader.close();

    return nullptr;
}

bool SceneParser::validateLine(const std::vector<std::string> &tokens, KeywordInfo& infoBuffer)
{
    const std::string strKey = tokens[0];
    auto it = s_KeyMap.find(strKey);

    if(tokens.empty() || tokens[0].empty() || tokens[0][0] == '#') return true;

    if(it == s_KeyMap.end()) {
        std::cout << "[ERROR] Key '" << strKey << "' is not defined in .scene language" << std::endl;
        return false;
    }
    const KeywordInfo info = it->second;

    if(info.isObject == readingObject()) {
        info.isObject ?
            std::cout << "[ERROR] Unable to create an object inside of another" << std::endl :
            std::cout << "[ERROR] Unable to define attributes outside of an object" << std::endl;
        return false;
    }

    if(info.length != tokens.size() - 1) {
        std::cout << "[ERROR] " << info.length << " parameters expected but " << tokens.size() - 1
            << " were given for attribute '" << strKey << "'" << std::endl;
        return false;
    }

    if(info.isObject) m_inObject = true;
    if(info.key == OBJ_END) m_inObject = false;

    infoBuffer = info;
    return true;
}

std::vector<float> SceneParser::parseLineAsFloats(const std::string& tokens, const KeywordInfo& info)
{
    std::vector<float> elements;
    for(unsigned int i = 1; i < tokens.size(); ++i) {
        elements.push_back(std::stof(&tokens[i]));
    }
    return elements;
}

std::vector<std::string> SceneParser::parseLineAsStrings(const std::string& tokens, const KeywordInfo& info)
{
    std::vector<std::string> elements;
    for(unsigned int i = 1; i < tokens.size(); ++i) {
        elements.push_back(&tokens[i]);
    }
    return elements;
}

void SceneParser::createCamera(const std::vector<std::string>& tokens, Scene* scene)
{
}
