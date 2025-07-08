#include "TextureManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void TextureManager::loadTexture(const std::string& name, const std::string textureFile)
{
    this->m_textures.emplace(name, 0);
    unsigned int* texture = &this->m_textures.find(name)->second;

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(textureFile.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "[ERROR] Failed to load texture '" << textureFile << "'" << std::endl;
    }
    stbi_image_free(data);
}

const unsigned int* TextureManager::getTexture(const std::string& name)
{
    auto it = this->m_textures.find(name);
    if(it != m_textures.end()) {
        return &it->second;
    }
    std::cout << "[WARNING] Texture '" + name + "' not found" << std::endl;
    return nullptr;
}