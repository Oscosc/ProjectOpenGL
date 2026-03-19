#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ProjectIGAI/core/Singleton.hpp>

class TextureManager : public Singleton<TextureManager>
{
    // INFO : friend permet d'accéder aux valeurs protected de la classe Singleton
    friend class Singleton<TextureManager>;

public:

    GLuint loadTexture(const std::string& path, const int mode = GL_REPEAT);

    GLuint loadTextureFromMemory(const unsigned char* dataBuffer, int length, const std::string& cacheKey, const int mode = GL_REPEAT);

private:

    /** Texture manager constructor (private for Singleton) */
    TextureManager() = default;

    /** textures map */
    std::unordered_map<std::string, GLuint> m_textures;
};