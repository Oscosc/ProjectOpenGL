#pragma once

#include <string>
#include <unordered_map>

#include "Singleton.hpp"
#include "../includes/shader.hpp"

class ShaderManager : public Singleton<ShaderManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<ShaderManager>;

public:
    void loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    Shader* getShader(const std::string& name);

private:
    ShaderManager() = default;
    std::unordered_map<std::string, Shader> m_shaders;
};