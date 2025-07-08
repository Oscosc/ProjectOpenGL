#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include "Singleton.hpp"

class TextureManager : public Singleton<TextureManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<TextureManager>;

public:

    void loadTexture(const std::string& name, const std::string textureFile);
    const unsigned int* getTexture(const std::string& name);

private:

    TextureManager() = default;
    std::unordered_map<std::string, unsigned int> m_textures;
};