#pragma once

#include <string>
#include <unordered_map>

#include <ProjectIGAI/core/Singleton.hpp>
#include <ProjectIGAI/graphics/IResourceManager.hpp>
#include <extern/shader.hpp>

struct ShaderParam : public ResourceParam {
    std::string vertexFile;
    std::string fragmentFile;

    unsigned int pointLights;
    unsigned int dirLights;
    unsigned int spotLights;

    ShaderParam(const std::string& name,
        const std::string& vertexFile,
        const std::string& fragmentFile,
        const unsigned int pointLights = 0,
        const unsigned int dirLights = 0,
        const unsigned int spotLights = 0) {
            this->name = name;
            this->vertexFile = vertexFile;
            this->fragmentFile = fragmentFile;
            this->pointLights = pointLights;
            this->dirLights = dirLights;
            this->spotLights = spotLights;
        }
};

/**
 * @brief Singleton class for shaders management in an application.
 */
class ShaderManager : public IResourceManager<Shader>, public Singleton<ShaderManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<ShaderManager>;

public:

    /**
     * @brief Compile and load a shader in the manager.
     * 
     */
    void loadResource(const ResourceParam& params) override;

private:

    /** Default constructor (private because of singleton) */
    ShaderManager() = default;
};