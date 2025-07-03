#include "ShaderManager.hpp"

void ShaderManager::loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
    this->m_shaders.emplace(name, Shader(vertexPath.c_str(), fragmentPath.c_str()));
}

Shader* ShaderManager::getShader(const std::string &name)
{
    auto it = this->m_shaders.find(name);
    if(it != m_shaders.end()) {
        return &it->second;
    }
    std::cout << "[WARNING] Shader '" + name + "' not found" << std::endl;
    return nullptr;
}

