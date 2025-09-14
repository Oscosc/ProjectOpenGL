#pragma once

#include <iostream>
#include <vector>

#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/Sphere.hpp>
#include <ProjectIGAI/core/BaseWindow.hpp>

class Mesh;

class RayTracer
{
public:
    RayTracer(unsigned int width, unsigned int height, GLFWwindow* window, Scene* scene);

    void draw(Scene* scene);

    void resetAccumulation();

private:

    struct GPUVertex {
        // Structure correspondant à l'alignement du standard 430 pour les layouts GLSL
        alignas(16) glm::vec3 position; // 16 octets
        alignas(16) glm::vec3 normal;   // 16 octets
        alignas(8)  glm::vec2 uv;       //  8 octets
        float _padding[2];              //  8 octets
                                // TOTAL : 48 octets
    };

    struct GPUMaterial {
        // Structure correspondant à l'alignement du standard 430 pour les layouts GLSL
        alignas(16) glm::vec3 ambient;  // 16 octets
        alignas(16) glm::vec3 diffuse;  // 16 octets
        alignas(16) glm::vec3 specular; // 16 octets
        float shininess;                //  4 octets
                                // TOTAL : 52 octets
    };

    struct GPUIndex {
        int vertPos;
        int matPos;
    };

    void initFullScreenQuad(GLFWwindow* window);

    GLuint createAccumulationTexture(int width, int height);
    GLuint createFBO(GLuint texture);
    
    void passSceneToGPU(Scene* scene);
    template <typename T> void createSSBO(GLuint& SSBO, const GLuint binding, const std::vector<T>& data);
    void addMeshToData(std::vector<GPUVertex>& vertices,
                       std::vector<GPUMaterial>& materials,
                       std::vector<GPUIndex>& indexes,
                       const Mesh* mesh
    );

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