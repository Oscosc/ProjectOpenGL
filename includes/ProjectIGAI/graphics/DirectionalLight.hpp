#pragma once

#include <ProjectIGAI/graphics/Light.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>

#define SHADOW_MAP_SIZE 1024

/**
 * @brief Class representing a directional light object.
 * 
 */
class DirectionalLight : public Light
{
public:

    /**
     * @brief Construct a new Directional Light
     * 
     * @param direction direction of the light
     * @param material material of the light
     */
    DirectionalLight(Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        LightProperties material = DEFAULT_LIGHT_PROPERTIES)
    : Light(transform, name, material)
    {
        // Shader instanciation
        ShaderManager::getInstance().loadResource(ShaderParam("shadow", "shaders/shadow.vs", "shaders/shadow.fs"));
        m_shadowShader = ShaderManager::getInstance().getResource("shadow");

        glGenFramebuffers(1, &m_depthMapFBO);

        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~DirectionalLight()
    {
        glDeleteFramebuffers(1, &m_depthMapFBO);
        glDeleteTextures(1, &m_depthMap);
    }

    void computeLightSpaceMatrix()
    {
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);
        
        glm::quat q = glm::quat(glm::radians(m_transform.rotation));
        glm::vec3 lightDir = glm::normalize(q * glm::vec3(0.0f, 0.0f, -1.0f));
        
        glm::vec3 lightPos = -lightDir * 10.0f;
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        
        m_lightSpaceMatrix = lightProjection * lightView;
    }

    glm::mat4 getLightSpaceMatrix() const { return m_lightSpaceMatrix; }
    GLuint getDepthMap() const { return m_depthMap; }
    Shader* getShadowShader() const { return m_shadowShader; }

    void beginShadowPass()
    {
        glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_shadowShader->use();
        m_shadowShader->setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
    }
    
private:

    GLuint m_depthMapFBO;
    GLuint m_depthMap;
    glm::mat4 m_lightSpaceMatrix;

    Shader* m_shadowShader;
};