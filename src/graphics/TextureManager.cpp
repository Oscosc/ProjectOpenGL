#include <ProjectIGAI/graphics/TextureManager.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <extern/stb_image.h>
#include <ProjectIGAI/core/Logger.hpp>

GLuint TextureManager::loadTexture(const std::string& path)
{
    unsigned int texture;

    if(m_textures.find(path) != m_textures.end()) {
        return m_textures[path];
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 3); // 3 -> Force 3 canaux de couleurs en mémoire
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        Logger::logError("Failed to load texture '" + path + "'");
    }
    stbi_image_free(data);

    m_textures[path] = texture;
    return texture;
}