#include <ProjectIGAI/core/Application.hpp>

#include <ProjectIGAI/core/SceneParser.hpp>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/Callbacks.hpp>
#include <ProjectIGAI/graphics/ShaderManager.hpp>
#include <ProjectIGAI/graphics/TextureManager.hpp>
#include <ProjectIGAI/graphics/CubemapManager.hpp>
#include <ProjectIGAI/graphics/PointLight.hpp>
#include <ProjectIGAI/core/RasterWindow.hpp>

// ------------------------------------------------------------------------------------------------
#include <chrono>
namespace Timer {
    using Time = std::chrono::steady_clock;
    using float_sec = std::chrono::duration<float>;
    using float_time_point = std::chrono::time_point<Time, float_sec>;

    float_time_point
    getCurrentTime() {
        return Time::now();
    }
}
// ------------------------------------------------------------------------------------------------

Application::Application(const unsigned int screenWidth, const unsigned int screenWeight) :
    m_screenWidth(screenWidth), m_screenHeight(screenWeight), m_activeWindowsCount(0)
{
    this->m_scene = new Scene();
}

void Application::initGLContext()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Application::initMainWindow()
{
    this->setMainWindow(new RasterWindow(m_scene, this->m_screenWidth, this->m_screenHeight, "Projet IGAI", NULL));
    if (getMainWindow() == NULL)
    {
        Logger::logError("Failed to create GLFW window");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(this->getMainWindow()->getGLFWwindow());

    // If no main window is already defined, update the counter
    if(m_activeWindowsCount == 0) m_activeWindowsCount++;
}

void Application::initGLComponents()
{
    // glfwSetWindowUserPointer(this->getMainWindow(), this);
    glfwSetInputMode(this->getMainWindow()->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::logError("Failed to initialize GLAD");
        glfwTerminate();
        exit(-1);
    }

    // OpenGL debbuging view (GL 4.3 and more)
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar* message, const void* userParam) {
        std::cerr << "GL CALLBACK (" << id << "): " << message << std::endl;
    }, nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // For cubemaps
    glViewport(0, 0, this->m_screenWidth, this->m_screenHeight);
}

void Application::initShaders(const std::string& sceneFile)
{
    std::ifstream stream(sceneFile);
    const json scene = json::parse(stream);

    auto sceneCount = SceneParser::retrieveSceneCounts(scene);
    unsigned int pointLights = sceneCount[SceneParser::POINT_LIGHT];
    unsigned int dirLights = sceneCount[SceneParser::DIR_LIGHT];
    unsigned int spotLights = sceneCount[SceneParser::SPOT_LIGHT];

    ShaderManager::getInstance().loadResource(ShaderParam("lighted", "shaders/lighted.vs", "shaders/lighted.fs", pointLights, dirLights, spotLights));
    ShaderManager::getInstance().loadResource(ShaderParam("quad", "shaders/quad.vs", "shaders/quad.fs"));
    ShaderManager::getInstance().loadResource(ShaderParam("cubemap", "shaders/cubemap.vs", "shaders/cubemap.fs"));

    CubemapManager::getInstance().loadResource(CubemapParam("Bell-Tower",
        "resources/cubemaps/bell_tower_8k.hdr"), 512);
}

void Application::initScene(const std::string& sceneFile)
{
    // Loading scene
    SceneParser::parseScene(this->m_scene, sceneFile);

    // Associating scene to main window
    this->getMainWindow()->setSceneRef(m_scene);

    this->getActiveCamera()->Ratio = (float)getScreenWidth() / (float)getScreenHeight();
}

void Application::postInitComponents()
{
    getMainWindow()->postInitProcess();
}

void Application::loop()
{
    while(!applicationShouldClose()) {
        // Processing inputs
        Callbacks::processInput(this);

        // Updating scene camera PV
        m_scene->updateActiveCameraPV();

        // Rendering each window
        for(unsigned int i = 0; i < m_activeWindowsCount; i++) {
            m_windows[i]->render();
        }
        
        // Poolling callbacks events
        glfwPollEvents();
    }
}

void Application::run(const std::string& sceneFile)
{
    auto timerStart = Timer::getCurrentTime();
    float execTime;

    // --------------------------------------------------------------------------------------------
    timerStart = Timer::getCurrentTime();
    initGLContext();
    execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logLoading("(" + std::to_string(execTime) + " ms)\tOpenGL correctly loaded");

    // --------------------------------------------------------------------------------------------
    timerStart = Timer::getCurrentTime();
    initMainWindow();
    execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logLoading("(" + std::to_string(execTime) + " ms)\tMain window correctly created");

    // --------------------------------------------------------------------------------------------
    timerStart = Timer::getCurrentTime();
    initGLComponents();
    execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logLoading("(" + std::to_string(execTime) + " ms)\t\tOpenGL/GLAD components correctly loaded");

    // --------------------------------------------------------------------------------------------
    timerStart = Timer::getCurrentTime();
    initShaders(sceneFile);
    execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logLoading("(" + std::to_string(execTime) + " ms)\tShaders correctly loaded and computed");

    // --------------------------------------------------------------------------------------------
    timerStart = Timer::getCurrentTime();
    initScene(sceneFile);
    execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logLoading("(" + std::to_string(execTime) + " ms)\tScene correctly loaded");

    // --------------------------------------------------------------------------------------------
    timerStart = Timer::getCurrentTime();
    postInitComponents();
    execTime = (Timer::getCurrentTime() - timerStart).count() * 1000.0;
    Logger::logLoading("(" + std::to_string(execTime) + " ms)\t\tAll components fully initialized");
    
    // --------------------------------------------------------------------------------------------
    Logger::logInfo("Starting application loop");
    loop();
    Logger::logInfo("Application closed");
}

bool Application::applicationShouldClose()
{

    // Check external windows calls
    for(unsigned int i = 1; i < m_activeWindowsCount; i++) {
        if(glfwWindowShouldClose(getExternalWindow(i)->getGLFWwindow())) {
            this->cleanRemoveExternalWindow(i);
            Logger::logInfo("External window " + std::to_string(i) + " correctly closed");
        }
    }
    
    // Main window
    return glfwWindowShouldClose(getMainWindow()->getGLFWwindow());
}


BaseWindow* Application::getExternalWindow(unsigned int windowID) const
{
    if(windowID <= 0 || windowID >= m_activeWindowsCount) {
        Logger::logWarning("Window ID doesn't exist or is 0. Returning main window by default");
        return getMainWindow();
    }
    return m_windows[windowID];
}

void Application::cleanRemoveExternalWindow(unsigned int windowID)
{
    glfwDestroyWindow(getExternalWindow(windowID)->getGLFWwindow());
    for(int i = windowID; i < m_activeWindowsCount - 1; i++) {
        m_windows[i] = m_windows[i + 1];
    }
    m_activeWindowsCount--;
    m_windows[m_activeWindowsCount] = nullptr;
}

Application* Application::getApplicationFromWindow(GLFWwindow *window)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if(!app) {
        Logger::logWarning("Unable to load application from user pointer");
        return nullptr;
    }
    return app;
}