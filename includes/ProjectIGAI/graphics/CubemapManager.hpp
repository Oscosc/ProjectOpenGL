#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <glad/glad.h>

#include <ProjectIGAI/graphics/IResourceManager.hpp>

class Scene;

struct CubemapParam : public ResourceParam {
    std::string path;

    CubemapParam(const std::string name, const std::string path) {
        this->name = name;
        this->path = path;
    }
};

struct Cubemap {
    GLuint environment;
    GLuint irradiance;
};

class CubemapManager : public IResourceManager<Cubemap>, public Singleton<CubemapManager>
{
    friend class Singleton<CubemapManager>;

public:

    void loadResource(const ResourceParam& params) override { loadResource(params, 512); }

    void loadResource(const ResourceParam& params, int resolution);

    void drawCubemap(const std::string& name, Scene* scene);

private:

    /** Cubemap manager constructor (private for Singleton) */
    CubemapManager();

    /** Cubemap VAO/VBO */
    unsigned int m_VAO;
    unsigned int m_VBO;

    /** TEMPORARY */
    void renderCube();
    unsigned int cubeVAO;
    unsigned int cubeVBO;
};