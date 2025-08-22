#include "Application.hpp"

#include "SceneParser.hpp"
#include "Logger.hpp"
#include "Callbacks.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "Mesh.hpp"
#include "Sphere.hpp"
#include "PointLight.hpp"
#include "BezierCurve.hpp"
#include "BezierSurface.hpp"
#include "Grid.hpp"

#include <chrono>
#define timer std::chrono::high_resolution_clock
#define duration std::chrono::duration_cast<std::chrono::nanoseconds>

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
        Logger::logError("Failed to create GLFW window");
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
        Logger::logError("Failed to initialize GLAD");
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

void Application::initShaders(const std::string& sceneFile)
{
    auto sceneCount = SceneParser::retrieveSceneCounts(sceneFile);
    unsigned int pointLights = sceneCount[SceneParser::POINT_LIGHT];
    unsigned int dirLights = sceneCount[SceneParser::DIR_LIGHT];
    unsigned int spotLights = sceneCount[SceneParser::SPOT_LIGHT];

    ShaderManager::getInstance().loadShader("lighted", "shaders/lighted.vs", "shaders/lighted.fs", pointLights, dirLights, spotLights);
    ShaderManager::getInstance().loadShader("monochrome", "shaders/monochrome.vs", "shaders/monochrome.fs");
    ShaderManager::getInstance().loadShader("quad", "shaders/quad.vs", "shaders/quad.fs");
    ShaderManager::getInstance().loadShader("uv", "shaders/uv.vs", "shaders/uv.fs");
    ShaderManager::getInstance().loadShader("grid", "shaders/grid.vs", "shaders/grid.fs");

#ifdef LOAD_TEXTURES_ON
    TextureManager::getInstance().loadTexture("earth", "resources/8k_earth.jpg");
    TextureManager::getInstance().loadTexture("ceres", "resources/4k_ceres.jpg");
    TextureManager::getInstance().loadTexture("metal", "resources/4k_metal.jpg");
#endif
}

void Application::initScene(const std::string& file)
{
    this->m_scene = new Scene(SceneParser::parseScene(file));
    this->getActiveCamera()->Ratio = (float)getScreenWidth() / (float)getScreenHeight();

    // this->m_scene->addObject(new Grid());

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

    Logger::logInfo("Scene builded with " + std::to_string(m_scene->objectsCount()) + " visible objects in it");
}

void Application::initHUD()
{
    this->m_HUD = new HUD(getScreenWidth(), getScreenHeight());
}

void Application::loop()
{
    std::vector<unsigned int> timeUpdateMeasures;
    timeUpdateMeasures.resize(100);
    std::vector<unsigned int> inputProcessMeasures;
    inputProcessMeasures.resize(100);
    std::vector<unsigned int> flushingMeasures;
    flushingMeasures.resize(100);
    std::vector<unsigned int> sceneRenderMeasures;
    sceneRenderMeasures.resize(100);
    std::vector<unsigned int> hudRenderMeasures;
    hudRenderMeasures.resize(100);
    std::vector<unsigned int> bufferSwapMeasures;
    bufferSwapMeasures.resize(100);

    unsigned int counter = 0;

    while(!glfwWindowShouldClose(this->m_window)) {
        auto startTime = timer::now();

        /* ---TIME UPDATING---- */
        // TODO : Update to Time class
        float currentFrame = static_cast<float>(glfwGetTime());
        this->m_deltaTime = currentFrame - this->m_lastFrame;
        this->m_lastFrame = currentFrame;
        auto timeUpdateTimer = timer::now();

        /* --INPUT PROCESSING-- */
        Callbacks::processInput(this->m_window);
        auto inputProcessTimer = timer::now();

        /* ----FLUSHING OLD---- */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto flushingTimer = timer::now();

        /* --RENDERING SCENE--- */
        this->m_scene->render();
        auto sceneRenderTimer = timer::now();

        /* ---RENDERING HUD---- */
        this->m_HUD->render();
        auto hudRenderTimer = timer::now();

        /* --SWAPPING BUFFERS-- */
        glfwSwapBuffers(this->m_window);
        glfwPollEvents();
        auto bufferSwapTimer = timer::now();

        /* --MEASUREMENTS UPDATE-- */
        if(counter < 100) {
            timeUpdateMeasures[counter] = duration(timeUpdateTimer - startTime).count();
            inputProcessMeasures[counter] = duration(inputProcessTimer - timeUpdateTimer).count();
            flushingMeasures[counter] = duration(flushingTimer - inputProcessTimer).count();
            sceneRenderMeasures[counter] = duration(sceneRenderTimer - flushingTimer).count();
            hudRenderMeasures[counter] = duration(hudRenderTimer - sceneRenderTimer).count();
            bufferSwapMeasures[counter] = duration(bufferSwapTimer - hudRenderTimer).count();
            counter++;
        }
        else if(counter == 100) {
            Logger::logPerf("Average time for time updating : " + std::to_string(mean(timeUpdateMeasures)) + " µs");
            Logger::logPerf("Average time for input processing : " + std::to_string(mean(inputProcessMeasures)) + " µs");
            Logger::logPerf("Average time for flushing buffers : " + std::to_string(mean(flushingMeasures)) + " µs");
            Logger::logPerf("Average time for scene rendering : " + std::to_string(mean(sceneRenderMeasures)) + " µs");
            Logger::logPerf("Average time for HUD rendering : " + std::to_string(mean(hudRenderMeasures)) + " µs");
            Logger::logPerf("Average time for bufferSwapping : " + std::to_string(mean(bufferSwapMeasures)) + " µs");
            counter++;
        }
    }

    glfwTerminate();
}

void Application::run(const std::string& sceneFile)
{

    initWindow();
    Logger::logInfo("OpenGL Window correctly loaded");

    initGLComponents();
    Logger::logInfo("OpenGL/GLAD components correctly loaded");

    initCallbacks();
    Logger::logInfo("Callbacks correctly instancied");

    initShaders(sceneFile);
    Logger::logInfo("Shaders correctly loaded and computed");

    initScene(sceneFile);
    Logger::logInfo("Scene correctly loaded");

    initHUD();
    Logger::logInfo("HUD correctly computed");
    
    Logger::logInfo("Starting application loop");
    loop();
    Logger::logInfo("Application closed");
}

Application *Application::getApplicationFromWindow(GLFWwindow *window)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if(!app) {
        Logger::logWarning("Unable to load application from user pointer");
        return nullptr;
    }
    return app;
}
