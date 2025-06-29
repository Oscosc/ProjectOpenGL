#include "Application.hpp"

Application::Application() : m_screenWidth(DEFAULT_SCREEN_WIDTH), m_screenHeight(DEFAULT_SCREEN_HEIGHT)
{
}

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
}

void Application::initScene()
{
    Camera* initialCam = new Camera(glm::vec3(2.5f, 4.0f, 12.0f));
    initialCam->Ratio = (float)getScreenWidth() / (float)getScreenHeight();

    this->m_scene = new Scene(initialCam);
    
    this->m_scene->addObject(new Mesh("resources/teapot.obj"));
    this->m_scene->getObject(0)->setMaterial({
        ShaderManager::getInstance().getShader("lighted"),
        glm::vec3(1.0f)
    });

    this->m_scene->addObject(new Sphere(0.2, {0, 5, 0}));

    this->m_scene->addLight(new PointLight(
        {0.f, 5.f, 0.f},
        {1.f, 1.f, 1.f},
        0.5f
    ));
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

void Application::run()
{
    initWindow();
    initGLComponents();
    initCallbacks();
    initShaders();
    initScene();
    initHUD();
    
    loop();
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
