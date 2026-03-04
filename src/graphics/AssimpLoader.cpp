#include <ProjectIGAI/graphics/AssimpLoader.hpp>
#include <iostream>
#include <ProjectIGAI/geometry/Geometry.hpp>
#include <ProjectIGAI/graphics/StandardPBRMaterial.hpp>
#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/graphics/GeometryManager.hpp>
#include <ProjectIGAI/graphics/TextureManager.hpp>
#include <ProjectIGAI/core/Logger.hpp>
#include <extern/assimp_glm_helpers.h>

Node* AssimpLoader::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
    std::string extension = path.substr(path.find_last_of(".") + 1);
    if (extension == "obj" || extension == "OBJ") {
        importFlags |= aiProcess_FlipUVs;
    }
    const aiScene* scene = importer.ReadFile(path, importFlags);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    
    Node* rootNode = new Node(DEFAULT_TRANSFORM, path);

    std::map<std::string, BoneInfo> boneInfoMap;
    int boneCount = 0;

    std::string directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, rootNode, directory, boneInfoMap, boneCount);

    if (scene->HasAnimations()) {
        aiAnimation* firstAnimation = scene->mAnimations[0];
        Animation* animation = new Animation(firstAnimation, scene->mRootNode, boneInfoMap, boneCount); 
        Animator* animator = new Animator(animation);
        rootNode->setAnimator(animator);
    }
    
    return rootNode;
}

void AssimpLoader::processNode(aiNode *assimpNode, const aiScene *scene, Node *parentNode, std::string dir,
    std::map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
{
    // Transformation construction ----------------------------------------------------------------
    aiMatrix4x4 aiMat = assimpNode->mTransformation;
    aiVector3D aiScale, aiPosition;
    aiQuaternion aiRotationQuat;
    aiMat.Decompose(aiScale, aiRotationQuat, aiPosition);

    glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
    glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
    glm::quat glmQuat(aiRotationQuat.w, aiRotationQuat.x, aiRotationQuat.y, aiRotationQuat.z);
    glm::vec3 rotationRadians = glm::eulerAngles(glmQuat);
    glm::vec3 rotationDegrees = glm::degrees(rotationRadians);

    Transform localTransform = { position, scale, rotationDegrees };
    // --------------------------------------------------------------------------------------------

    Node* localNode = new Node(localTransform, assimpNode->mName.C_Str());
    parentNode->addChildren(*localNode);

    for(unsigned int m = 0; m < assimpNode->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[assimpNode->mMeshes[m]];
        
        // Geometry Extraction --------------------------------------------------------------------
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for(unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vertex;
            
            vertex.position = glm::vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);

            if (mesh->HasNormals()) {
                vertex.normal = glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            }

            if(mesh->mTextureCoords[0]) {
                vertex.uv = glm::vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
                vertex.tangent = glm::vec3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
            } else {
                vertex.uv = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for(unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }

        // Bones extraction (SKELETAL ANIMATION) --------------------------------------------------
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            } else {
                boneID = boneInfoMap[boneName].id;
            }

            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                vertices[vertexId].addBoneData(boneID, weight);
            }
        }
        // ----------------------------------------------------------------------------------------

        std::string name = dir + "::" + assimpNode->mName.C_Str();
        Geometry* geo = GeometryManager::getInstance().getRawGeometry(name, vertices, indices);
        // ----------------------------------------------------------------------------------------

        // Material extraction --------------------------------------------------------------------
        StandardPBRMaterial* mat = new StandardPBRMaterial();
        
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* assimpMat = scene->mMaterials[mesh->mMaterialIndex];

            aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
            if (aiReturn_SUCCESS == assimpMat->Get(AI_MATKEY_BASE_COLOR, color) || 
                aiReturn_SUCCESS == assimpMat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
                mat->albedo = glm::vec3(color.r, color.g, color.b);
            }

            float roughnessFactor = 1.0f;
            if (aiReturn_SUCCESS == assimpMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor)) {
                mat->roughness = roughnessFactor;
            }

            float metallicFactor = 0.0f;
            if (aiReturn_SUCCESS == assimpMat->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor)) {
                mat->metallic = metallicFactor;
            }

            struct TextureResult {
                std::string path;
                GLuint id = 0;
            };

            auto getTextureInfo = [&](aiTextureType type) -> TextureResult {
                TextureResult res;
                aiString str;
                if (assimpMat->GetTexture(type, 0, &str) == aiReturn_SUCCESS) {

                    const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
                    
                    if (embeddedTexture) {

                        std::string cacheKey = dir + "::" + str.C_Str();
                        if (embeddedTexture->mHeight == 0) {

                            res.id = TextureManager::getInstance().loadTextureFromMemory(
                                reinterpret_cast<const unsigned char*>(embeddedTexture->pcData),
                                embeddedTexture->mWidth,
                                cacheKey
                            );
                        }
                    } else {
                        res.path = dir + "/" + str.C_Str();
                    }
                }
                return res;
            };

            // ALBEDO
            TextureResult albedoRes = getTextureInfo(aiTextureType_BASE_COLOR);
            if (albedoRes.path.empty() && albedoRes.id == 0) albedoRes = getTextureInfo(aiTextureType_DIFFUSE);
            if (albedoRes.id > 0) mat->setAlbedoMapID(albedoRes.id);
            else if (!albedoRes.path.empty()) mat->setAlbedoTexture(albedoRes.path);

            // ROUGHNESS
            TextureResult roughnessRes = getTextureInfo(aiTextureType_DIFFUSE_ROUGHNESS);
            if (roughnessRes.id > 0) mat->setRoughnessMapID(roughnessRes.id);
            else if (!roughnessRes.path.empty()) mat->setRoughnessTexture(roughnessRes.path);

            // METALLIC
            TextureResult metallicRes = getTextureInfo(aiTextureType_METALNESS);
            if (metallicRes.path.empty() && metallicRes.id == 0) metallicRes = getTextureInfo(aiTextureType_SPECULAR);
            if (metallicRes.id > 0) mat->setMetallicMapID(metallicRes.id);
            else if (!metallicRes.path.empty()) mat->setMetallicTexture(metallicRes.path);

            // NORMAL
            TextureResult normalRes = getTextureInfo(aiTextureType_NORMALS);
            if (normalRes.path.empty() && normalRes.id == 0) normalRes = getTextureInfo(aiTextureType_HEIGHT);
            if (normalRes.id > 0) mat->setNormalMapID(normalRes.id);
            else if (!normalRes.path.empty()) mat->setNormalTexture(normalRes.path);

            // AMBIENT OCCLUSION
            TextureResult aoRes = getTextureInfo(aiTextureType_AMBIENT_OCCLUSION);
            if (aoRes.path.empty() && aoRes.id == 0) aoRes = getTextureInfo(aiTextureType_LIGHTMAP);
            if (aoRes.id > 0) mat->setAOMapID(aoRes.id);
            else if (!aoRes.path.empty()) mat->setAOTexture(aoRes.path);
        }
        // ----------------------------------------------------------------------------------------
        
        Object* meshObject = new Object(mesh->mName.C_Str());
        meshObject->setGeometry(geo);
        meshObject->setMaterial(mat);
        
        localNode->addChildren(*meshObject);
    }

    for(unsigned int i = 0; i < assimpNode->mNumChildren; i++) {
        processNode(assimpNode->mChildren[i], scene, localNode, dir, boneInfoMap, boneCount);
    }
}