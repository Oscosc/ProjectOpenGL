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
    passSceneToGPU(scene);

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
    std::vector<Sphere*> spheres = scene->getSpheresRT();
    unsigned int i = 0;
    for(Sphere* obj : spheres) {
        m_shader_compute->setVec3("u_spheres[" + std::to_string(i) + "].position", obj->getTransform().position);
        m_shader_compute->setFloat("u_spheres[" + std::to_string(i) + "].radius", obj->getRadius());
        m_shader_compute->setVec3("u_spheres[" + std::to_string(i) + "].material.color", obj->getMaterial().matShader.color);
        m_shader_compute->setFloat("u_spheres[" + std::to_string(i) + "].material.roughness", obj->getMaterial().matShader.roughness);
        m_shader_compute->setFloat("u_spheres[" + std::to_string(i) + "].material.metallic", obj->getMaterial().matShader.metallic);

        ++i;
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

void RayTracer::passSceneToGPU(Scene *scene)
{
    // Setup shader target
    m_shader_compute->use();

    // Create each buffers for layouts
    std::vector<GPUVertex>   vertices  = std::vector<GPUVertex>();
    std::vector<GPUMaterial> materials = std::vector<GPUMaterial>();
    std::vector<GPUIndex>    indexes   = std::vector<GPUIndex>();

    // Add each mesh informations to the buffer
    // (NE FONCTIONNE PAS DANS LA BOUCLE)
    for(unsigned int i = 0; i < scene->objectsCount(); ++i) {
        Mesh* refMesh = dynamic_cast<Mesh*>(scene->getObject(i));
        if(refMesh != nullptr) {
            addMeshToData(vertices, materials, indexes, refMesh);
        }
    }
    
    // DEBUG : GPU usage
    GLint maxSize;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSize);
    GLuint usedMemory = vertices.size()  * sizeof(GPUVertex)
                      + materials.size() * sizeof(GPUMaterial)
                      + indexes.size()   * sizeof(GPUIndex);
    float memoryRatio = usedMemory / (float)maxSize;
    Logger::logPerf("GPU memory usage : " + std::to_string(memoryRatio) + "% ("
                + std::to_string(usedMemory) + " / " + std::to_string(maxSize) + ")");

    // Bind data of each layout to GPU
    createSSBO<GPUVertex>(m_verticesSSBO, 0, vertices);
    createSSBO<GPUMaterial>(m_materialsSSBO, 1, materials);
    createSSBO<GPUIndex>(m_indexesSSBO, 2, indexes);
}

template <typename T>
void RayTracer::createSSBO(GLuint &SSBO, const GLuint binding, const std::vector<T> &data)
{
    glGenBuffers(1, &SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 data.size() * sizeof(T),
                 data.data(),
                 GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTracer::addMeshToData(std::vector<GPUVertex> &vertices,
                              std::vector<GPUMaterial> &materials,
                              std::vector<GPUIndex> &indexes,
                              const Mesh* mesh)
{
    // Save index offset of the layouts
    unsigned int verticesLayout = vertices.size();
    unsigned int materialsLayout = materials.size();

    // Compute usefull values for the nex steps
    glm::mat4 modelMatrix = mesh->getModelMatrix();
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    // Vertices part
    std::vector<Vertex> tmp = mesh->getVertices();
    for(Vertex vert : tmp) {
        // Swap to GPUVertex object
        GPUVertex tmpGPU = {vert.position, vert.normal, vert.uv};
        tmpGPU.position = glm::vec3(modelMatrix * glm::vec4(tmpGPU.position, 1.f));
        tmpGPU.normal = glm::normalize(normalMatrix * vert.normal);

        // Add to vertices tab
        vertices.push_back(tmpGPU);
    }

    // Materials part
    ShaderMaterial mat = mesh->getMaterial().matShader;
    // Swap to GPUMaterial object
    GPUMaterial matGPU = {mat.color, mat.roughness, mat.metallic};
    // Add to materials tab
    materials.push_back(matGPU);

    // Indexes part
    std::vector<unsigned int> indVertex = mesh->getIndexes();
    for(unsigned int e : indVertex) {
        indexes.push_back({
            static_cast<int>(e + verticesLayout),
            static_cast<int>(materialsLayout)
        });
    }
}