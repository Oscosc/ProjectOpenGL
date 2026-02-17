#pragma once

#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>

class Scene;

class Material
{
public:

    Material(Shader* shader) : m_shader(shader) {}

    virtual void bind(Scene* scene) = 0;

    Shader* getShader() const { return m_shader; }

protected:

    Shader* m_shader;
};