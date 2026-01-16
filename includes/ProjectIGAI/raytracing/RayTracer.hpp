#pragma once

#include <iostream>
#include <vector>

#include <extern/shader.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/Sphere.hpp>
#include <ProjectIGAI/core/BaseWindow.hpp>

class Mesh;

/**
 * @brief Ray-tracer class. This class can be viewed as a smart bridge between CPU and GPU.
 * It handle vertices and materials GPU-memory alignment and drawn an image in a scene with
 * progressive ray-tracing (to avoid lag).
 */
class RayTracer
{
public:

    /**
     * @brief Construct a new ray-tracer.
     * 
     * @param width width of the reference window
     * @param height height of the reference window
     * @param window GLFWwindow object associated
     * @param scene scene to render/draw during loop
     */
    RayTracer(unsigned int width, unsigned int height, GLFWwindow* window, Scene* scene);

    /**
     * @brief Drawing function. Send all formatted data to shader for drawing, and then retrieve
     * computed image to store it.
     * 
     * @param scene scene to draw
     */
    void draw(Scene* scene);

    /**
     * @brief Reset the accumulated value of the image (if a camera movement occured for example)
     */
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
        float roughness;                //  4 octets
        float metallic;                 //  4 octets
                                // TOTAL : 24 octets
    };


    struct GPUIndex {
        // Structure contenant les index associés des materiaux/vertex
        int vertPos;
        int matPos;
    };

    /**
     * @brief Create a fullscreen quad at object creation.
     * This quad will be used to render the scene, since all the job is done by a shader.
     * 
     * @param window target window for the quad
     */
    void initFullScreenQuad(GLFWwindow* window);

    /**
     * @brief Create an accumulation texture for progressive ray-tracing.
     * 
     * @param width texture width
     * @param height texture height
     * @return ID of the created texture
     */
    GLuint createAccumulationTexture(int width, int height);

    /**
     * @brief Create a FBO to capture image between shader computation and screen drawing (for
     * progressive ray-tracing).
     * 
     * @param texture ID of the texture to match the FBO.
     * @return FBO's ID
     */
    GLuint createFBO(GLuint texture);
    
    /**
     * @brief Send the scene informations to the GPU with formatted data as GPU*Structures*.
     * 
     * @param scene Scene to send
     */
    void passSceneToGPU(Scene* scene);

    /**
     * @brief Create a SSBO (Shared Storage Buffer Object) for large data transfert to GPU.
     * 
     * @tparam T Type of the SSBO
     * @param SSBO GLuint which will store the SSBO ID
     * @param binding Binding attribute
     * @param data Data to store and share un the SSBO
     */
    template <typename T> void createSSBO(GLuint& SSBO, const GLuint binding, const std::vector<T>& data);

    /**
     * @brief Transform Mesh informations to readable data for the GPU (i.e. std::vector of
     * GPU Structures)
     * 
     * @param vertices empty vector for vertices storage
     * @param materials empty vector for materials storage
     * @param indexes empty vector for indexes storage
     * @param mesh Mesh to transform as readable GPU structures
     */
    void addMeshToData(std::vector<GPUVertex>& vertices,
                       std::vector<GPUMaterial>& materials,
                       std::vector<GPUIndex>& indexes,
                       const Mesh* mesh
    );

    /** Progressive ray-tracing associated objects */
    GLuint m_textureA, m_textureB;
    GLuint m_fboA, m_fboB;
    int m_frameCount;

    /** Quad associated objects */
    GLuint m_VAO;
    GLuint m_VBO;

    /** Large data storage associated objects */
    GLuint m_verticesSSBO;
    GLuint m_indexesSSBO;
    GLuint m_materialsSSBO;

    /** Displaying objects */
    Shader* m_shader_compute;
    Shader* m_shader_display;
    GLFWwindow* m_window;
};