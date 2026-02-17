#pragma once

#include <extern/shader.hpp>
#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>

class Material
{
public:

    Material(Shader* shader) : m_shader(shader) {}

    virtual void bind(Scene* scene) = 0;

    Shader* getShader() const { return m_shader; }

protected:

    Shader* m_shader;
};


class StandardPBRMaterial : public Material
{
public:

    StandardPBRMaterial() : Material(ShaderManager::getInstance().getResource("lighted")) {}

    glm::vec3 albedo = glm::vec3(1.0);
    float roughness = 0.5f;
    float metallic = 0.0f;

    void bind(Scene* scene) override
    {
        m_shader->use();

        m_shader->setVec3("material.albedo", albedo);
        m_shader->setFloat("material.roughness", roughness);
        m_shader->setFloat("material.metallic", metallic);
    }

private:

    Shader* m_shader;

};