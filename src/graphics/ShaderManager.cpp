#include <ProjectIGAI/graphics/ShaderManager.hpp>

#include <ProjectIGAI/core/Logger.hpp>

void ShaderManager::loadResource(const ResourceParam &params)
{
    const ShaderParam* shParams = dynamic_cast<const ShaderParam*>(&params);
    this->m_resources.emplace(shParams->name,
        Shader(
            shParams->vertexFile.c_str(),
            shParams->fragmentFile.c_str(),
            shParams->pointLights,
            shParams->dirLights,
            shParams->spotLights
        )
    );
}