#include "Application.hpp"
#include "SceneParser.hpp"

// #define LOAD_TEXTURES

Application::Application(const unsigned int screenWidth, const unsigned int screenWeight) :
    m_screenWidth(screenWidth), m_screenHeight(screenWeight)
{
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    this->m_window = glfwCreateWindow(this->m_screenWidth, this->m_screenHeight, "Projet IGAI", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "[ERROR] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(this->m_window);
}

void Application::initGLComponents()
{
    glfwSetWindowUserPointer(this->m_window, this);
    glfwSetInputMode(this->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "[ERROR] Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, this->m_screenWidth, this->m_screenHeight);
}

void Application::initCallbacks()
{
    glfwSetFramebufferSizeCallback(this->m_window, Callbacks::framebuffer_size_callback);
    glfwSetCursorPosCallback(this->m_window, Callbacks::mouse_callback);
    glfwSetScrollCallback(this->m_window, Callbacks::scroll_callback);
    glfwSetKeyCallback(this->m_window, Callbacks::key_callback);
    glfwSetMouseButtonCallback(this->m_window, Callbacks::mouse_button_callback);
}

void Application::initShaders()
{
    ShaderManager::getInstance().loadShader("lighted", "shaders/lighted.vs", "shaders/lighted.fs");
    ShaderManager::getInstance().loadShader("monochrome", "shaders/monochrome.vs", "shaders/monochrome.fs");
    ShaderManager::getInstance().loadShader("quad", "shaders/quad.vs", "shaders/quad.fs");
    ShaderManager::getInstance().loadShader("uv", "shaders/uv.vs", "shaders/uv.fs");

#ifdef LOAD_TEXTURES
    TextureManager::getInstance().loadTexture("earth", "resources/8k_earth.jpg");
    TextureManager::getInstance().loadTexture("ceres", "resources/4k_ceres.jpg");
    TextureManager::getInstance().loadTexture("metal", "resources/4k_metal.jpg");
#endif
}

void Application::initScene(const std::string& file)
{
    this->m_scene = new Scene(SceneParser::parseScene(file));
    this->getActiveCamera()->Ratio = (float)getScreenWidth() / (float)getScreenHeight();

#ifdef RAY_TRACING_ON

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = randomFloat();
            glm::vec3 center(a + 0.9*randomFloat(), 0.2, b + 0.9*randomFloat());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = glm::vec3(randomFloat(), randomFloat(), randomFloat()) * glm::vec3(randomFloat(), randomFloat(), randomFloat());
                    this->m_scene->addObject(new Sphere(
                        0.2,
                        {center, glm::vec3(0.f), glm::vec3(1.f)},
                        {ShaderManager::getInstance().getShader("monochrome"), albedo, glm::vec3(0.5f), glm::vec3(0.5f), 32.f}
                    ));
                    Sphere* sphere = dynamic_cast<Sphere*>(this->m_scene->getObject(this->m_scene->objectsCount() - 1));
                    sphere->Type = HitType::DIFFUSE;

                    Sphere* sphere2 = dynamic_cast<Sphere*>(this->m_scene->getObject(this->m_scene->objectsCount() - 1));
                    assert(sphere2->Type == HitType::DIFFUSE);

                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = glm::vec3(randomFloat(), randomFloat(), randomFloat());
                    this->m_scene->addObject(new Sphere(
                        0.2,
                        {center, glm::vec3(0.f), glm::vec3(1.f)},
                        {ShaderManager::getInstance().getShader("monochrome"), albedo, glm::vec3(0.5f), glm::vec3(0.5f), randomFloat(0.f, 0.5f)}
                    ));
                    Sphere* sphere = dynamic_cast<Sphere*>(this->m_scene->getObject(this->m_scene->objectsCount() - 1));
                    sphere->Type = HitType::METAL;
                    
                    Sphere* sphere2 = dynamic_cast<Sphere*>(this->m_scene->getObject(this->m_scene->objectsCount() - 1));
                    assert(sphere2->Type == HitType::METAL);

                } else {
                    // glass
                    this->m_scene->addObject(new Sphere(
                        0.2,
                        {center, glm::vec3(0.f), glm::vec3(1.f)},
                        {ShaderManager::getInstance().getShader("monochrome"), glm::vec3(0.5f), glm::vec3(0.5f), glm::vec3(0.5f), 1.5f}
                    ));
                    Sphere* sphere = dynamic_cast<Sphere*>(this->m_scene->getObject(this->m_scene->objectsCount() - 1));
                    sphere->Type = HitType::GLASS;

                    Sphere* sphere2 = dynamic_cast<Sphere*>(this->m_scene->getObject(this->m_scene->objectsCount() - 1));
                    assert(sphere2->Type == HitType::GLASS);
                }
            }
        }
    }

#endif

    std::cout << "[INFO] Scene builded with " << m_scene->objectsCount() << " visible objects in it" << std::endl;
}

void Application::initHUD()
{
    this->m_HUD = new HUD(getScreenWidth(), getScreenHeight());
}

void Application::loop()
{
    while(!glfwWindowShouldClose(this->m_window)) {
        /* ---TIME UPDATING---- */
        // TODO : Update to Time class
        float currentFrame = static_cast<float>(glfwGetTime());
        this->m_deltaTime = currentFrame - this->m_lastFrame;
        this->m_lastFrame = currentFrame;

        /* --INPUT PROCESSING-- */
        Callbacks::processInput(this->m_window);

        /* ----FLUSHING OLD---- */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* --RENDERING SCENE--- */
        this->m_scene->render();

        /* ---RENDERING HUD---- */
        this->m_HUD->render();

        /* --SWAPPING BUFFERS-- */
        glfwSwapBuffers(this->m_window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Application::run(const std::string& sceneFile)
{
    initWindow();
    std::cout << "[INFO] OpenGL Window correctly loaded" << std::endl;

    initGLComponents();
    std::cout << "[INFO] OpenGL/GLAD components correctly loaded" << std::endl;

    initCallbacks();
    std::cout << "[INFO] Callbacks correctly instancied" << std::endl;

    initShaders();
    std::cout << "[INFO] Shaders correctly loaded and computed" << std::endl;

    initScene(sceneFile);
    std::cout << "[INFO] Scene correctly loaded" << std::endl;

    initHUD();
    std::cout << "[INFO] HUD correctly computed" << std::endl;
    
    std::cout << "[INFO] Starting application loop" << std::endl;
    loop();

    std::cout << "[INFO] Application closed" << std::endl;
}

Application *Application::getApplicationFromWindow(GLFWwindow *window)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if(!app) {
        std::cout << "[WARNING] Unable to load application from user pointer" << std::endl;
        return nullptr;
    }
    return app;
}
