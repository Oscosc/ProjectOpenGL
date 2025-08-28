#pragma once

#include <iostream>
#include <vector>

#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>

class RayTracer
{
public:
    RayTracer(unsigned int width, unsigned int height, GLFWwindow* window) {
        initFullScreenQuad(window);
        m_shader = ShaderManager::getInstance().getShader("ray-tracing");
        m_shader->use();
        m_shader->setVec2("u_resolution", width, height);
    }

    void draw() {
        m_shader->use();
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

private:
    void initFullScreenQuad(GLFWwindow* window) {
        glfwMakeContextCurrent(window);

        float vertices[] = {
            -1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }

    GLuint m_VAO;
    GLuint m_VBO;
    Shader* m_shader;
};