#pragma once

#include <ProjectIGAI/core/BaseWindow.hpp>
#include <ProjectIGAI/raytracing/RayTracer.hpp>

class RaytracingWindow : public BaseWindow
{
public:

    RaytracingWindow(Scene* refScene,
        const unsigned int width = DEFAULT_WINDOW_WIDTH,
        const unsigned int height = DEFAULT_WINDOW_HEIGHT,
        const std::string& title = DEFAULT_WINDOW_NAME,
        GLFWwindow* rootWindow = nullptr);

    void postInitProcess() override;

private:
    
    RayTracer* m_rayTracer;

    void subClassRendering() override;
};