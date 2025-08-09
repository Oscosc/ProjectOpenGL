#include "ShaderManager.hpp"
#include "Logger.hpp"

void ShaderManager::loadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath,
    const unsigned int pointLight, const unsigned int dirLight, const unsigned int spotLight)
{
    this->m_shaders.emplace(name, Shader(vertexPath.c_str(), fragmentPath.c_str(), pointLight, dirLight, spotLight));
}

Shader *ShaderManager::getShader(const std::string &name)
{
    auto it = this->m_shaders.find(name);
    if(it != m_shaders.end()) {
        return &it->second;
    }
    Logger::logWarning("Shader '" + name + "' not found");
    return nullptr;
}

