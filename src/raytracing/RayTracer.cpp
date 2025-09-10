#include <ProjectIGAI/raytracing/RayTracer.hpp>

RayTracer::RayTracer(unsigned int width, unsigned int height, GLFWwindow* window) :
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
        m_shader_compute->setVec3("spheres[" + std::to_string(i) + "].position", obj->getTransform().position);
        m_shader_compute->setFloat("spheres[" + std::to_string(i) + "].radius", obj->getRadius());
        m_shader_compute->setVec3("spheres[" + std::to_string(i) + "].material.ambient", obj->getMaterial().matShader.ambient);
        m_shader_compute->setVec3("spheres[" + std::to_string(i) + "].material.diffuse", obj->getMaterial().matShader.diffuse);
        m_shader_compute->setVec3("spheres[" + std::to_string(i) + "].material.specular", obj->getMaterial().matShader.specular);
        m_shader_compute->setFloat("spheres[" + std::to_string(i) + "].material.shininess", obj->getMaterial().matShader.shininess);
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