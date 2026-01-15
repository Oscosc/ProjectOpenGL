#pragma once

#include <ProjectIGAI/core/BaseWindow.hpp>

/**
 * @brief Window for displaying a scene using rasterization processes.
 * Main loop and shaders used for this window are based on LearnOpenGL
 * basecode.
 */
class RasterWindow : public BaseWindow
{
public:

    /**
     * @brief Construct a new RasterWindow object
     * 
     * @param width width of the window
     * @param height height of the window
     * @param title title of the window
     * @param rootWindow root window to share context with
     */
    RasterWindow(Scene* refScene,
        const unsigned int width = DEFAULT_WINDOW_WIDTH,
        const unsigned int height = DEFAULT_WINDOW_HEIGHT,
        const std::string& title = DEFAULT_WINDOW_NAME,
        GLFWwindow* rootWindow = nullptr);
    
    /**
     * @brief Init HUD (UI) object on this window.
     * HUD is created as a quad on the screen, and a shader draw the quad on it
     */
    void postInitProcess() override;

    /**
     * @brief Create an HUD object associated with this window.
     */
    void initHUD();
    
    /**
     * @brief Override the resize function to update cursor HUD ratio
     * 
     * @param width width of the screen
     * @param height height if the screen
     */
    void onResize(int width, int height) override;
    
private:
    
    /**
     * @brief Simply draw all object using their base draw() function
     * 
     * @param scene scene to render
     */
    void subClassRendering() override;
    
    /** HUD of this window (green crosshair) */
    HUD* m_HUD;
};