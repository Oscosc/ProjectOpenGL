#pragma once

#include <iostream>
#include <vector>

#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/Sphere.hpp>

class RayTracer
{
public:
    RayTracer(unsigned int width, unsigned int height, GLFWwindow* window) {
        initFullScreenQuad(window);
        m_shader = ShaderManager::getInstance().getShader("ray-tracing");
        m_shader->use();
        m_shader->setVec2("u_resolution", width, height);
    }

    void draw(Scene* scene) {
        Camera* cameraRef = scene->getActiveCamera();

        m_shader->use();

        // Camera setting
        m_shader->setVec3("camera.position", cameraRef->Position);
        m_shader->setVec3("camera.forward", cameraRef->Front);
        m_shader->setVec3("camera.up", cameraRef->Up);
        m_shader->setFloat("camera.fov", glm::radians(cameraRef->Fov));

        // Objects setting
        Sphere* obj = static_cast<Sphere*>(scene->getObject(0));
        m_shader->setVec3("sphere.position", obj->getTransform().position);
        m_shader->setFloat("sphere.radius", obj->getRadius());
        m_shader->setVec3("sphere.material.ambient", obj->getMaterial().matShader.ambient);
        m_shader->setVec3("sphere.material.diffuse", obj->getMaterial().matShader.diffuse);
        m_shader->setVec3("sphere.material.specular", obj->getMaterial().matShader.specular);
        m_shader->setFloat("sphere.material.shininess", obj->getMaterial().matShader.shininess);

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