#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

#include <ProjectIGAI/core/Singleton.hpp>

class Scene;

class CubemapManager : public Singleton<CubemapManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<CubemapManager>;

public:

    void loadCubemap(const std::string& name);

    void drawCubemap(const std::string& name, Scene* scene);

    unsigned int getCubemap(const std::string& name) { return m_cubemaps.at(name); }

private:

    /** Cubemap manager constructor (private for Singleton) */
    CubemapManager();

    /** Cubemaps list */
    std::unordered_map<std::string, unsigned int> m_cubemaps;

    /** Cubemap VAO/VBO */
    unsigned int m_VAO;
    unsigned int m_VBO;
};