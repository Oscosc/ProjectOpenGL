#include <ProjectIGAI/raytracing/RayTracer.hpp>

RayTracer::RayTracer(unsigned int width, unsigned int height, GLFWwindow* window) : m_window(window) {
    initFullScreenQuad(window);
    m_shader = ShaderManager::getInstance().getShader("ray-tracing");
    m_shader->use();
    m_shader->setVec2("u_resolution", width, height);
}

void RayTracer::draw(Scene* scene) {
    Camera* cameraRef = scene->getActiveCamera();

    m_shader->use();

    // Time
    BaseWindow* win = static_cast<BaseWindow*>(glfwGetWindowUserPointer(m_window));
    m_shader->setFloat("u_time", win->getDeltaTime());

    // Camera setting
    m_shader->setVec3("camera.position", cameraRef->Position);
    m_shader->setVec3("camera.forward", cameraRef->Front);
    m_shader->setVec3("camera.up", cameraRef->Up);
    m_shader->setFloat("camera.fov", glm::radians(cameraRef->Fov));

    // Objects setting
    for(int i = 0; i < 2; i++) {
        Sphere* obj = static_cast<Sphere*>(scene->getObject(i));
        m_shader->setVec3("spheres[" + std::to_string(i) + "].position", obj->getTransform().position);
        m_shader->setFloat("spheres[" + std::to_string(i) + "].radius", obj->getRadius());
        m_shader->setVec3("spheres[" + std::to_string(i) + "].material.ambient", obj->getMaterial().matShader.ambient);
        m_shader->setVec3("spheres[" + std::to_string(i) + "].material.diffuse", obj->getMaterial().matShader.diffuse);
        m_shader->setVec3("spheres[" + std::to_string(i) + "].material.specular", obj->getMaterial().matShader.specular);
        m_shader->setFloat("spheres[" + std::to_string(i) + "].material.shininess", obj->getMaterial().matShader.shininess);
    }

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RayTracer::initFullScreenQuad(GLFWwindow* window) {
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