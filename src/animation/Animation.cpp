#include <ProjectIGAI/animation/Animation.hpp>

Animation::Animation(const aiAnimation* animation, const aiNode* rootNode, std::map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
{
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 24.0f;
    
    m_BoneInfoMap = boneInfoMap;

    ReadHierarchyData(m_RootNode, rootNode);
    ReadMissingBones(animation, boneInfoMap, boneCount);
}

Animation::~Animation()
{
}

Bone* Animation::FindBone(const std::string& name)
{
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.GetBoneName() == name;
        }
    );
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

void Animation::ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
{
    int size = animation->mNumChannels;
    
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }
    m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}