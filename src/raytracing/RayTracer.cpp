#include <ProjectIGAI/raytracing/RayTracer.hpp>

#include <ProjectIGAI/graphics/Mesh.hpp>

RayTracer::RayTracer(unsigned int width, unsigned int height, GLFWwindow* window, Scene* scene) :
    m_window(window), m_frameCount(1)
{
    // Init quad rendering object
    initFullScreenQuad(window);

    // Init texture system
    m_textureA = createAccumulationTexture(width, height);
    m_textureB = createAccumulationTexture(width, height);

    m_fboA = createFBO(m_textureA);
    m_fboB = createFBO(m_textureB);

    // Shader for ray-tracing logic
    m_shader_compute = ShaderManager::getInstance().getShader("ray-tracing-compute");
    m_shader_compute->use();
    m_shader_compute->setVec2("u_resolution", width, height);

    // Shader for texture display
    m_shader_display = ShaderManager::getInstance().getShader("ray-tracing-display");

    // Load data to GPU
    createAndLoadSSBO(scene);

}

void RayTracer::draw(Scene* scene) {
    Camera* cameraRef = scene->getActiveCamera();

    // FrameBuffer & textures setup ---------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboB);
    m_shader_compute->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureA);
    m_shader_compute->setInt("u_previousFrame", 0);
    m_shader_compute->setInt("u_frameCount", m_frameCount);

    // RENDERING LOGIC ----------------------------------------------------------------------------

    // Time
    BaseWindow* win = static_cast<BaseWindow*>(glfwGetWindowUserPointer(m_window));
    m_shader_compute->setFloat("u_time", win->getDeltaTime());

    // Camera setting
    m_shader_compute->setVec3("camera.position", cameraRef->Position);
    m_shader_compute->setVec3("camera.forward", cameraRef->Front);
    m_shader_compute->setVec3("camera.up", cameraRef->Up);
    m_shader_compute->setFloat("camera.fov", glm::radians(cameraRef->Fov));

    // Objects setting
    for(int i = 0; i < 5; i++) {
        Sphere* obj = static_cast<Sphere*>(scene->getObject(i));
        m_shader_compute->setVec3("u_spheres[" + std::to_string(i) + "].position", obj->getTransform().position);
        m_shader_compute->setFloat("u_spheres[" + std::to_string(i) + "].radius", obj->getRadius());
        m_shader_compute->setVec3("u_spheres[" + std::to_string(i) + "].material.ambient", obj->getMaterial().matShader.ambient);
        m_shader_compute->setVec3("u_spheres[" + std::to_string(i) + "].material.diffuse", obj->getMaterial().matShader.diffuse);
        m_shader_compute->setVec3("u_spheres[" + std::to_string(i) + "].material.specular", obj->getMaterial().matShader.specular);
        m_shader_compute->setFloat("u_spheres[" + std::to_string(i) + "].material.shininess", obj->getMaterial().matShader.shininess);
    }

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_shader_display->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureB);
    m_shader_display->setInt("uTexture", 0);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    std::swap(m_textureA, m_textureB);
    std::swap(m_fboA, m_fboB);

    m_frameCount++;
}

void RayTracer::resetAccumulation() {
    m_frameCount = 1;

    // Nettoyer les textures (remplir avec noir)
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fboB);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RayTracer::initFullScreenQuad(GLFWwindow* window) {
    glfwMakeContextCurrent(window);

    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
        
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

GLuint RayTracer::createAccumulationTexture(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbinding texture
    return texture;
}

GLuint RayTracer::createFBO(GLuint texture)
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Creation check
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::logError("Incomplete FBO !");
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

void RayTracer::createAndLoadSSBO(Scene *scene)
{
    struct Index {
        int vertPos;
        int matPos;
    };

    struct GPUVertex {
        // Structure correspondant à l'alignement du standard 430 pour les layouts GLSL
        alignas(16) glm::vec3 position; // 16 octets
        alignas(16) glm::vec3 normal;   // 16 octets
        alignas(8)  glm::vec2 uv;       //  8 octets
        float _padding[2];              //  8 octets
                                // TOTAL : 48 octets
    };

    m_shader_compute->use();

    Mesh* refMesh = dynamic_cast<Mesh*>(scene->getObject(0));
    if(refMesh == nullptr) return;

    // VERTICES BUFFER SETUP
    std::vector<Vertex> tmp = refMesh->getVertices();
    glm::mat4 modelMatrix = refMesh->getModelMatrix();
    std::vector<GPUVertex> vertices = std::vector<GPUVertex>();
    for(Vertex vert : tmp) {
        GPUVertex tmpGPU = {vert.position, vert.normal, vert.uv};
        tmpGPU.position = modelMatrix * glm::vec4(tmpGPU.position, 1.f);

        vertices.push_back(tmpGPU);
    }

    glGenBuffers(1, &m_verticesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_verticesSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 vertices.size() * sizeof(GPUVertex),
                 vertices.data(),
                 GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_verticesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    // INDEXES BUFFER SETUP
    std::vector<unsigned int> indVertex = refMesh->getIndexes();
    std::vector<Index> indexes = std::vector<Index>();
    for(unsigned int e : indVertex) {
        indexes.push_back({static_cast<int>(e), 0});
    }

    glGenBuffers(1, &m_indexesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexesSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 indexes.size() * sizeof(Index),
                 indexes.data(),
                 GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_indexesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    // MATERIAL BUFFER SETUP
    std::vector<ShaderMaterial> materials = std::vector<ShaderMaterial>();
    materials.push_back(refMesh->getMaterial().matShader);

    glGenBuffers(1, &m_materialsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 materials.size() * sizeof(ShaderMaterial),
                 materials.data(),
                 GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_materialsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::cout << "Nb indexes : " << indexes.size() << std::endl;
    for(int i = 0; i < indexes.size(); i+=3) {
        std::cout << "TRIANGLE " << i / 3 << std::endl;
        std::cout << "  - Vertice A : " << glm::to_string(vertices.at(indexes.at(i+0).vertPos).position) << std::endl;
        std::cout << "  - Vertice B : " << glm::to_string(vertices.at(indexes.at(i+1).vertPos).position) << std::endl;
        std::cout << "  - Vertice C : " << glm::to_string(vertices.at(indexes.at(i+2).vertPos).position) << std::endl;
        std::cout << std::endl;
    }
}