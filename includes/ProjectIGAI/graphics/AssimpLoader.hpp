#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ProjectIGAI/graphics/Node.hpp>
#include <ProjectIGAI/animation/AnimData.hpp>

class AssimpLoader
{
public:

    static Node* loadModel(const std::string& path);
    static void processNode(aiNode* assimpNode, const aiScene* scene, Node* parentNode, std::string dir,
        std::map<std::string, BoneInfo>& boneInfoMap, int& boneCount);

};