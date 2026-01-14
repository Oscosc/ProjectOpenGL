#pragma once

#include <ProjectIGAI/core/BaseWindow.hpp>
#include <ProjectIGAI/raytracing/RayTracer.hpp>

/**
 * @brief Window that's display the scene usign RayTracing algorithms.
 * 
 * @warning This class isn't fully working. Need to integrate RayTracing to the whole project
 */
class RaytracingWindow : public BaseWindow
{
public:

    /**
     * @brief Construct a new Raytracing Window object.
     * 
     * @param refScene Scene to display
     * @param width window width
     * @param height window height
     * @param title window title
     * @param rootWindow parent window (null if main window)
     */
    RaytracingWindow(Scene* refScene,
        const unsigned int width = DEFAULT_WINDOW_WIDTH,
        const unsigned int height = DEFAULT_WINDOW_HEIGHT,
        const std::string& title = DEFAULT_WINDOW_NAME,
        GLFWwindow* rootWindow = nullptr);
    
    /**
     * @brief Create a new Raytracer to draw the scene with raytracing.
     */
    void postInitProcess() override;
    
    /**
     * @brief Return the rayTracer associated with this window.
     */
    RayTracer* getRayTracer() { return m_rayTracer; }

private:
    
    /** Raytracer of this window */
    RayTracer* m_rayTracer;
    
    /** Render the window by tracing rays in the scene */
    void subClassRendering() override;
};