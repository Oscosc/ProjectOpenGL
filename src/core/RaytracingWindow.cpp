#include <ProjectIGAI/core/RaytracingWindow.hpp>

RaytracingWindow::RaytracingWindow(Scene *refScene, const unsigned int width, const unsigned int height,
    const std::string &title, GLFWwindow *rootWindow) : BaseWindow(refScene, width, height, title, rootWindow)
{
    // If not root, post init is immediate
    if(!m_isRoot) postInitProcess();
}

void RaytracingWindow::postInitProcess()
{
    Logger::logPerf("Entering post process init of RayTracer");
    m_rayTracer = new RayTracer(getScreenWidth(), getScreenHeight(), m_window);
    Logger::logPerf("Exiting post process init of RayTracer");
}

void RaytracingWindow::subClassRendering()
{
    m_rayTracer->draw();
}
