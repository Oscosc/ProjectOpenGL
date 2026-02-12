#include <ProjectIGAI/graphics/CubemapManager.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <extern/stb_image.h>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>

void CubemapManager::loadResource(const ResourceParam& params)
{
    std::vector<std::string> facesFiles = {
        "resources/cubemaps/" + params.name + "/right.jpg",
        "resources/cubemaps/" + params.name + "/left.jpg",
        "resources/cubemaps/" + params.name + "/top.jpg",
        "resources/cubemaps/" + params.name + "/bottom.jpg",
        "resources/cubemaps/" + params.name + "/front.jpg",
        "resources/cubemaps/" + params.name + "/back.jpg",
    };

    unsigned int textureID;
    glGenTextures(1, &textureID); // Création de la texture en mémoire
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // Dire à OpenGL : "Cette texture est une cubemap"

    int width, height, nrChannels;
    for(unsigned int i = 0; i < facesFiles.size(); i++) {
        unsigned char* data = stbi_load(facesFiles[i].c_str(), &width, &height, &nrChannels, 0);

        if(data) { // Image correctement chargée
            GLenum cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i; // Sélection de la bonne face
            glTexImage2D(cubeFace, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Création texture
            stbi_image_free(data);
        }
        else {
            Logger::logWarning("Failed to load cubemap element : " + facesFiles[i]);
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Ajout aux Cubemaps disponibles
    m_resources.emplace(params.name, textureID);
}

void CubemapManager::drawCubemap(const std::string &name, Scene* scene)
{
    glDepthFunc(GL_EQUAL);

    Shader* shader = ShaderManager::getInstance().getResource("cubemap");
    shader->use();

    glm::mat4 view = glm::mat4(glm::mat3(scene->getActiveCameraPV().view)); 
    shader->setMat4("view", view);
    shader->setMat4("projection", scene->getActiveCameraPV().projection);
    
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_resources.at(name));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

CubemapManager::CubemapManager()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &this->m_VAO);
    glGenBuffers(1, &this->m_VBO);

    glBindVertexArray(this->m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader* shader = ShaderManager::getInstance().getResource("cubemap");
    shader->use();
    shader->setInt("skybox", 0);
}