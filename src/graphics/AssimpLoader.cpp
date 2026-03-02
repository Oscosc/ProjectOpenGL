#include <ProjectIGAI/graphics/AssimpLoader.hpp>
#include <iostream>
#include <ProjectIGAI/geometry/Geometry.hpp>
#include <ProjectIGAI/graphics/StandardPBRMaterial.hpp>
#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/graphics/GeometryManager.hpp>
#include <ProjectIGAI/graphics/TextureManager.hpp>

Node *AssimpLoader::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    
    Node* rootNode = new Node(DEFAULT_TRANSFORM, path);

    std::string directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, rootNode, directory);
    
    return rootNode;
}

void AssimpLoader::processNode(aiNode *assimpNode, const aiScene *scene, Node *parentNode, std::string dir)
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

        std::string name = dir + "::" + assimpNode->mName.C_Str();
        Geometry* geo = GeometryManager::getInstance().getRawGeometry(name, vertices, indices);
        // ----------------------------------------------------------------------------------------

        // Material extraction --------------------------------------------------------------------
        StandardPBRMaterial* mat = new StandardPBRMaterial();
        
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* assimpMat = scene->mMaterials[mesh->mMaterialIndex];

            auto getTexturePath = [&](aiTextureType type) -> std::string {
                aiString str;
                if (assimpMat->GetTexture(type, 0, &str) == aiReturn_SUCCESS) {
                    return dir + "/" + str.C_Str();
                }
                return "";
            };

            // ALBEDO
            std::string albedoPath = getTexturePath(aiTextureType_BASE_COLOR);
            if (albedoPath.empty()) albedoPath = getTexturePath(aiTextureType_DIFFUSE);
            if (!albedoPath.empty()) mat->setAlbedoTexture(albedoPath);

            // ROUGHNESS
            std::string roughnessPath = getTexturePath(aiTextureType_DIFFUSE_ROUGHNESS);
            if (!roughnessPath.empty()) mat->setRoughnessTexture(roughnessPath);

            // METALLIC
            std::string metallicPath = getTexturePath(aiTextureType_METALNESS);
            if (!metallicPath.empty()) mat->setMetallicTexture(metallicPath);

            // NORMAL
            std::string normalPath = getTexturePath(aiTextureType_NORMALS);
            if (normalPath.empty()) normalPath = getTexturePath(aiTextureType_HEIGHT);
            if (!normalPath.empty()) mat->setNormalTexture(normalPath);

            // AMBIENT OCCLUSION
            std::string aoPath = getTexturePath(aiTextureType_AMBIENT_OCCLUSION);
            if (aoPath.empty()) aoPath = getTexturePath(aiTextureType_LIGHTMAP); 
            if (!aoPath.empty()) mat->setAOTexture(aoPath);
        }
        // ----------------------------------------------------------------------------------------
        
        Object* meshObject = new Object(mesh->mName.C_Str());
        meshObject->setGeometry(geo);
        meshObject->setMaterial(mat);
        
        localNode->addChildren(*meshObject);
    }

    for(unsigned int i = 0; i < assimpNode->mNumChildren; i++) {
        processNode(assimpNode->mChildren[i], scene, localNode, dir);
    }
}