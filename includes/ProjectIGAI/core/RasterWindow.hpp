#pragma once

#include <ProjectIGAI/core/BaseWindow.hpp>

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
    RasterWindow(const unsigned int width = DEFAULT_WINDOW_WIDTH,
        const unsigned int height = DEFAULT_WINDOW_HEIGHT,
        const std::string& title = DEFAULT_WINDOW_NAME,
        GLFWwindow* rootWindow = nullptr) :
        BaseWindow(width, height, title, rootWindow) {}
    
private:
    
    /**
     * @brief Simply draw all object using their base draw() function
     * 
     * @param scene scene to render
     */
    void subClassRendering(Scene* scene) override;
};