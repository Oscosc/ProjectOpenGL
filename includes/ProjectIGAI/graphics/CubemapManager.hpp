#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

#include <ProjectIGAI/graphics/IResourceManager.hpp>

class Scene;

struct CubemapParam : public ResourceParam {
    CubemapParam(const std::string name) {
        this->name = name;
    }
};

class CubemapManager : public IResourceManager<unsigned int>, public Singleton<CubemapManager>
{
    friend class Singleton<CubemapManager>;

public:

    void loadResource(const ResourceParam& params) override;

    void drawCubemap(const std::string& name, Scene* scene);

private:

    /** Cubemap manager constructor (private for Singleton) */
    CubemapManager();

    /** Cubemap VAO/VBO */
    unsigned int m_VAO;
    unsigned int m_VBO;
};