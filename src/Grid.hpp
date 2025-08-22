#pragma once

#include "Object.hpp"
#include "Logger.hpp"
#include "ShaderManager.hpp"

class Grid : public Object
{
public:
    Grid() : Object() {
        this->m_material.shader = ShaderManager::getInstance().getShader("grid");

        m_vertices = {
            {glm::vec3(-500.0f, 0.f, -500.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, 0.f)},
            {glm::vec3( 500.0f, 0.f, -500.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f)},
            {glm::vec3(-500.0f, 0.f,  500.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, 1.f)},
            {glm::vec3( 500.0f, 0.f,  500.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 1.f)}
        };
        m_indexes = {0, 1, 2, 1, 2, 3};

        initGLObject();
    }

    void draw(Scene* scene) const override {
        Shader* shader = this->getMaterial().shader;
        shader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(glm::mat4(1.0f), this->m_transform.position);
        model = glm::rotate(model, glm::radians(this->m_transform.rotation.x), glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(this->m_transform.rotation.y), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(this->m_transform.rotation.z), glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, this->m_transform.scale);

        shader->setMat4("model", model);
        shader->setMat4("view", scene->getActiveCameraPV().view);
        shader->setMat4("projection", scene->getActiveCameraPV().projection);

        updateMaterial(shader);

        scene->updateLigth(shader);

        glBindVertexArray(this->m_VAO);
        glDrawElements(GL_TRIANGLES, this->m_indexes.size(), GL_UNSIGNED_INT, (void*)0);

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            Logger::logError("in Grid : GLError " + std::to_string(err));
        }
    }

private:
};