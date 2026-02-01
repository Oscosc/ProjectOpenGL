#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

#include <ProjectIGAI/core/Singleton.hpp>

class TextureManager : public Singleton<TextureManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<TextureManager>;

public:

    /**
     * @brief Load a texture as an OpenGL object
     * 
     * @param name name of the texture in the program
     * @param textureFile file where the texture is stored
     */
    void loadTexture(const std::string& name, const std::string textureFile);

    /**
     * @brief Retrieve a texture by it's name stored in the TextureManager
     * 
     * @param name name of the texture
     * @return pointer to the texture object
     */
    unsigned int getTexture(const std::string& name) const;

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