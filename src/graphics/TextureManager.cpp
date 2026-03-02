#include <ProjectIGAI/graphics/TextureManager.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <extern/stb_image.h>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/core/utils.hpp>

GLuint TextureManager::loadTexture(const std::string& path, const int mode)
{
    unsigned int texture;

    if(m_textures.find(path) != m_textures.end()) {
        return m_textures[path];
    }

    auto timerStart = Timer::getCurrentTime();

    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1) {
            format = GL_RED;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        }
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        Logger::logError("Failed to load texture '" + path + "'");
    }
    stbi_image_free(data);

    float execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logPerf(std::to_string(execTime) + " ms for loading resource '" + path + "'");

    m_textures[path] = texture;
    return texture;
}