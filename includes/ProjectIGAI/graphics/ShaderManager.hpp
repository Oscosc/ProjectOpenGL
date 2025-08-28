#pragma once

#include <string>
#include <unordered_map>

#include <ProjectIGAI/core/Singleton.hpp>
#include <extern/shader.hpp>

/**
 * @brief Singleton class for shaders management in an application.
 */
class ShaderManager : public Singleton<ShaderManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<ShaderManager>;

public:

    /**
     * @brief Compile and load a shader in the manager.
     * 
     * @param name name of the shader (to retrieve it later)
     * @param vertexPath path of the .vs or .vert associated file
     * @param fragmentPath path of the .fs or .frag associated file
     */
    void loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath,
        const unsigned int pointLight = 0, const unsigned int dirLight = 0, const unsigned int spotLight = 0);

    /**
     * @brief Gives a reference to the shader by it's name (null if not found)
     * 
     * @param name name of the shader defined when loaded
     */
    Shader* getShader(const std::string& name);

private:

    /**
     * @brief Default constructor (private because of singleton)
     */
    ShaderManager() = default;
    
    std::unordered_map<std::string, Shader> m_shaders;
};