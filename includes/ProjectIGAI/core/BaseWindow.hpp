#pragma once

#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/core/Application.hpp>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_WINDOW_NAME "New Window"

/**
 * @brief Base class to create window that's handle scene informations differently.
 */
class BaseWindow
{
public:

    /**
     * @brief Construct a new Base Window object
     * 
     * @param width width of the window
     * @param height height of the window
     * @param title title of the window
     * @param rootWindow root window to share context with
     */
    BaseWindow(const unsigned int width = DEFAULT_WINDOW_WIDTH,
        const unsigned int height = DEFAULT_WINDOW_HEIGHT,
        const std::string& title = DEFAULT_WINDOW_NAME,
        GLFWwindow* rootWindow = nullptr);

    virtual ~BaseWindow() {}
    
    /**
     * @brief Link all standard callbacks to abstracts "onX()" functions for this window
     * 
     */
    void initCallbacks();

    /**
     * @brief Render the scene using the specific subClassRendering() function that's defined
     * in each class extending BaseWindow.
     * 
     * @param scene scene to render
     */
    void render(Scene* scene);
    
    /**
     * @brief Retrieve the GLFWwindow object reference associated with this window.
     * 
     * @return GLFWwindow of this window
     */
    GLFWwindow* getGLFWwindow() const { return m_window; }
    
    virtual void onResize(int width, int height);
    virtual void onKey(int key, int scancode, int action, int mods);
    
    virtual void onMouseButton(int button, int action, int mods) {} // No default behavior
    virtual void onCursorPos(double xpos, double ypos) {}           // No default behavior
    virtual void onScroll(double xOffset, double yOffset) {}        // No default behavior

protected:
    
    virtual void subClassRendering(Scene* scene) = 0;

    GLFWwindow* m_window;
    float m_lastFrame;
};