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
    RayTracer(unsigned int width, unsigned int height, GLFWwindow* window, Scene* scene);

    void draw(Scene* scene);

    void resetAccumulation();

private:

    void initFullScreenQuad(GLFWwindow* window);

    GLuint createAccumulationTexture(int width, int height);
    GLuint createFBO(GLuint texture);
    
    void createAndLoadSSBO(Scene* scene);

    GLuint m_textureA, m_textureB;
    GLuint m_fboA, m_fboB;
    int m_frameCount;

    GLuint m_VAO;
    GLuint m_VBO;

    GLuint m_verticesSSBO;
    GLuint m_indexesSSBO;
    GLuint m_materialsSSBO;

    Shader* m_shader_compute;
    Shader* m_shader_display;
    GLFWwindow* m_window;
};