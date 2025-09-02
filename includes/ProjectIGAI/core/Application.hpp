#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/core/BaseWindow.hpp>
#include <ProjectIGAI/graphics/HUD.hpp>
#include <ProjectIGAI/raytracing/RayTracer.hpp>

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

#define MAX_WINDOWS 2

// #define RAY_TRACING_ON
// #define LOAD_TEXTURES_ON

class BaseWindow; // For includes error

/**
 * @brief Application handle OpenGL initialization, as well as Shader loading, Scene loading and
 * callbacks loading. It provide an "user friendly" interface to just create an application in the
 * main and run it.
 */
class Application
{
public:

    /**
     * @brief Create an application object that will be displayed on the screen as a window.
     * 
     * @param screenWidth width of the application window in pixels (default = 800)
     * @param screenHeight height of the application wiondw in pixels (default = 600)
     */
    Application(
        const unsigned int screenWidth = DEFAULT_SCREEN_WIDTH,
        const unsigned int screenHeight = DEFAULT_SCREEN_HEIGHT
        );

    /**
     * @brief Default destructor. 
     */
    ~Application() = default;

    /**
     * @brief Initialize OpenGL context.
     */
    void initGLContext();

     /**
      * @brief Create main window.
      */
    void initMainWindow();

    /**
     * @brief Initialize GLAD and GL options/components.
     */
    void initGLComponents();

    /**
     * @brief References callbacks to the window.
     */
    void initCallbacks();

    /**
     * @brief Load and compile all default shader files into ShaderManager.
     */
    void initShaders(const std::string& sceneFile);

    /**
     * @brief Read scene file to construct scene. Scene files structure is
     * described in the README of the project.
     * 
     * @param file relative path from executable to the scene file to load
     */
    void initScene(const std::string& file);

    /**
     * @brief Instanciate all HUD components to be able to render them during
     * the application loop.
     */
    void initHUD();

    /**
     * @brief main loop of the application. This function is called in the run
     * function, after calling all init processes.
     * 
     * The main loop consist of the following steps :
     * - Updating frame/time information
     * - Processing inputs
     * - Flushing old screen content
     * - Rendering scene elements
     * - Rendering HUD
     * - Swapping buffers
     */
    void loop();

    /**
     * @brief function to run an application.
     * 
     * This function is normaly the only one called by user to display an application, providing
     * a full management of OpenGL initializations and Scene components initialization
     * 
     * @param sceneFile path of the JSON file containing informations about elements in the scene to
     * render.
     */
    void run(const std::string& sceneFile);

    /**
     * @brief Check if the application should close or not
     */
    bool applicationShouldClose();

    /**
     * @brief Return screen width.
     */
    const unsigned int getScreenWidth() const { return m_screenWidth; }

    /**
     * @brief Return screen height.
     */
    const unsigned int getScreenHeight() const { return m_screenHeight; }

    // TODO : Passer la gestion de la souris dans une classe

    /**
     * @brief Return false if mouse is enable (used as quad), true otherwise.
     */
    const bool isMouseActive() const { return m_mouseActive; }

    /**
     * @brief Switch value of mouse disponibility status.
     */
    void switchMouseActive() { m_mouseActive = !m_mouseActive; }

    /**
     * @brief Return true while mouse hasn't been linked to the quad. False after.
     */
    const bool isFirstMouse() const { return m_firstMouse; }

    /**
     * @brief Call this function when mouse link to the quad for the first time.
     * This function switch the status of the firstMouse variable to false.
     */
    void firstMouseDone() { m_firstMouse = false; }

    /**
     * @brief Give cursor position relatively to the center of the application window. 
     */
    const glm::vec2 getCursor() const { return m_cursor; }

    /**
     * @brief Set cursor position.
     * 
     * @param pos new position of the cursor.
     */
    void setCursor(glm::vec2 pos) { m_cursor = pos; }

    /**
     * @brief Set cursor position.
     * 
     * @param x new position on X-axis of the cursor.
     * @param y new position on Y-axis of the cursor.
     */
    void setCursor(float x, float y) { m_cursor = glm::vec2(x, y); }

    /**
     * @brief Gives the delta time between last frame and current frame. 
     */
    const float getDeltaTime() const { return m_deltaTime; }

    /**
     * @brief Set the new delta time for the current frame.
     * 
     * @param value new delta time value.
     */
    void setDeltaTime(float value) { m_deltaTime = value; }

    /**
     * @brief Return the main window of the application.
     */
    BaseWindow* getMainWindow() const { return m_windows[0]; }

    /**
     * @brief Set the Main Window of the application.
     * 
     * @param window new main window value
     */
    void setMainWindow(BaseWindow* window) { m_windows[0] = window; }

    /**
     * @brief Return external window of the application if exists.
     */
    BaseWindow* getExternalWindow(unsigned int windowID) const;

    /**
     * @brief Set a new external window for this application.
     * 
     * @param width width of the window
     * @param height height of the window
     * @param windowTitle title of the window
     * @return ID of the window created
     */
    unsigned int createExternalWindow(
        const unsigned int width = DEFAULT_SCREEN_WIDTH,
        const unsigned int height = DEFAULT_SCREEN_HEIGHT,
        const std::string& windowTitle = "New window"
    );

    void cleanRemoveExternalWindow(unsigned int windowID);

    /**
     * @brief return the active Camera of the current scene managed by this application.
     * 
     * @warning This function need to be removed, there is no justification to get Camera from
     * application context
     */
    Camera* getActiveCamera() const { return this->m_scene->getActiveCamera(); }

    Scene* getCurrentScene() const { return this->m_scene; }

    /**
     * @brief static function to retrive an application object linked to the window given in
     * parameter.
     * 
     * @param window reference window to retrieve Application from.
     */
    static Application* getApplicationFromWindow(GLFWwindow* window);

private:

    /* --- WINDOW --- */
    BaseWindow* m_windows[MAX_WINDOWS];
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
    unsigned int m_activeWindowsCount;

    /* -- RAY-TRACING -- */
    RayTracer* m_rt;

    /* --- TIME --- */
    float m_deltaTime;
    float m_lastFrame;

    /* --- RENDER --- */
    HUD* m_HUD;
    Scene* m_scene;

    /* --- UTILS --- */
    bool m_mouseActive = true;
    bool m_firstMouse = true;
    glm::vec2 m_cursor;
};

