#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

#include <ProjectIGAI/core/Singleton.hpp>
#include <ProjectIGAI/graphics/IResourceManager.hpp>

struct TextureParam : public ResourceParam {
    std::string file;

    TextureParam(const std::string name, const std::string file) {
        this->name = name;
        this->file = file;
    }
};

class TextureManager : public IResourceManager<unsigned int>, public Singleton<TextureManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<TextureManager>;

public:

    void loadResource(const ResourceParam& params) override;

    /**
     * @brief Get the Default white texture
     */
    unsigned int getDefaultTexture() { return m_defaultTexture; }

private:

    /** Texture manager constructor (private for Singleton) */
    TextureManager() = default;

    unsigned int createDefaultTexture();

    /** textures map */
    std::unordered_map<std::string, unsigned int> m_textures;

    unsigned int m_defaultTexture = createDefaultTexture();
};