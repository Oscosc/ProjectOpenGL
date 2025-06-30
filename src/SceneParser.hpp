#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "utils.hpp"

#define STD_SCENE_DELIMITER " "
#define OPENING_SCOPE "{"
#define CLOSING_SCOPE "}"

class Scene;

class SceneParser
{
public:
    enum Keyword {
        // Objects
        CAMERA, POINT_LIGHT, DIR_LIGHT, SPOT_LIGHT, SPHERE, MESH,

        // Strings
        FILE, SHADER,

        // Transform
        POSITION, ROTATION, SCALE, SIZE,

        // Appearance
        COLOR, STRENGTH,

        // Others
        OBJ_END,
    };

    enum NativeType {STRING, FLOAT, NONE};

    struct KeywordInfo {
        Keyword key;
        bool isObject;
        unsigned int length;
        NativeType type;
    };

    SceneParser() : m_inObject(false) {}

    Scene* parseScene(const std::string& file);
    bool validateLine(const std::vector<std::string>& tokens, KeywordInfo& infoBuffer);

    std::vector<float> parseLineAsFloats(const std::string& tokens, const KeywordInfo& info);
    std::vector<std::string> parseLineAsStrings(const std::string& tokens, const KeywordInfo& info);

    void createCamera(const std::vector<std::string>& tokens, Scene* scene, std::ifstream reader, std::string lineBuffer);
    void createPointLight();

    bool readingObject() { return m_inObject; }

private:
    bool m_inObject;

    static inline const std::unordered_map<std::string, KeywordInfo> s_KeyMap = {
        {"camera", {CAMERA, true, 1, NONE}},
        {"point_light", {POINT_LIGHT, true, 1, NONE}},
        {"dir_light", {DIR_LIGHT, true, 1, NONE}},
        {"spot_light", {SPOT_LIGHT, true, 1, NONE}},
        {"sphere", {SPHERE, true, 1, NONE}},
        {"mesh", {MESH, true, 1, NONE}},

        {"file", {FILE, false, 1, STRING}},
        {"shader", {SHADER, false, 1, STRING}},

        {"position", {POSITION, false, 3, FLOAT}},
        {"rotation", {ROTATION, false, 3, FLOAT}},
        {"scale", {SCALE, false, 3, FLOAT}},
        {"size", {SIZE, false, 1, FLOAT}},
        
        {"color", {COLOR, false, 3, FLOAT}},
        {"strength", {STRENGTH, false, 1, FLOAT}},

        {CLOSING_SCOPE, {OBJ_END, false, 0, NONE}},
    };
};