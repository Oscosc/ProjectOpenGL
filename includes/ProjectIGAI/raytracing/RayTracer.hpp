#pragma once

#include <iostream>
#include <vector>

#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/Sphere.hpp>
#include <ProjectIGAI/core/BaseWindow.hpp>

class RayTracer
{
public:
    RayTracer(unsigned int width, unsigned int height, GLFWwindow* window);

    void draw(Scene* scene);

private:
    void initFullScreenQuad(GLFWwindow* window);

    GLuint m_VAO;
    GLuint m_VBO;
    Shader* m_shader;
    GLFWwindow* m_window;
};