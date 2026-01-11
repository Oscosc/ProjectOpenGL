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
    BaseWindow(Scene* refScene,
        const unsigned int width = DEFAULT_WINDOW_WIDTH,
        const unsigned int height = DEFAULT_WINDOW_HEIGHT,
        const std::string& title = DEFAULT_WINDOW_NAME,
        GLFWwindow* rootWindow = nullptr);
    
    /**
     * @brief Virtual destructor for abstract class
     */
    virtual ~BaseWindow() {}
    
    /**
     * @brief Link all standard callbacks to abstracts "onX()" functions for this window
     */
    void initCallbacks();

    /**
     * @brief Call various functions that needs to be called after object/window creation.
     */
    virtual void postInitProcess() = 0;

    /**
     * @brief Render the scene using the specific subClassRendering() function that's defined
     * in each class extending BaseWindow.
     * 
     * @param scene scene to render
     */
    void render();
    
    /**
     * @brief Retrieve the GLFWwindow object reference associated with this window.
     * 
     * @return GLFWwindow of this window
     */
    GLFWwindow* getGLFWwindow() const { return m_window; }

    /**
     * @brief Return the scene
     */
    Scene* getScene() const { return m_scene; }

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
     * @brief Return true if this window is the main (i.e. root) window, false otherwise.
     */
    bool isRoot() { return m_isRoot; }

    /**
     * @brief Set the scene to display in the loop.
     * 
     * @param sceneRef 
     */
    void setSceneRef(Scene* sceneRef) { m_scene = sceneRef; }
    
    /**
     * @brief Callback for window resizing.
     * 
     * @param width new window width
     * @param height new window height
     */
    virtual void onResize(int width, int height);

    /**
     * @brief Callback when a key is pressed.
     * 
     * @param key id of the pressed key
     * @param scancode scancode of the pressed key
     * @param action action done on this key (pressed, released, etc...)
     * @param mods mods for this key (unused)
     */
    virtual void onKey(int key, int scancode, int action, int mods);

    /**
     * @brief Callback when scrolling.
     * 
     * @param xOffset X scroll movement
     * @param yOffset Y scroll movement
     */
    virtual void onScroll(double xOffset, double yOffset);

    /**
     * @brief Callback when cursor moves.
     * 
     * @param xPos new cursor X pos
     * @param yPos new cursor Y pos
     */
    virtual void onCursorPos(double xPos, double yPos);
    
    /**
     * @brief Callback when mouse button is clicked.
     * 
     * @param button button id (left, right, etc)
     * @param action action done on this key (pressed, released, etc...)
     * @param mods mods for this button (unused)
     */
    virtual void onMouseButton(int button, int action, int mods) {}

protected:
    
    /** Subclass rendering function to define in each child */
    virtual void subClassRendering() = 0;
    
    /** Scene displayed by this window */
    Scene* m_scene;

    /** GLFWwindow object wrapped in this Window object */
    GLFWwindow* m_window;
    
    /** Cursor position in this window */
    glm::vec2 m_cursor;
    
    /** Window width */
    unsigned int m_screenWidth;

    /** Window height */
    unsigned int m_screenHeight;

    /*--- Various attributes for FPS coherence ---*/
    float m_lastFrame;
    float m_deltaTime;
    
    /*--- Various attributes to determine window focus ---*/
    bool m_isRoot;
    bool m_mouseActive;
    bool m_firstMouse;
};