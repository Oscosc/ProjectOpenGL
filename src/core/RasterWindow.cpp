#include <ProjectIGAI/core/RasterWindow.hpp>

RasterWindow::RasterWindow(Scene *refScene, const unsigned int width, const unsigned int height,
    const std::string &title, GLFWwindow *rootWindow) : BaseWindow(refScene, width, height, title, rootWindow)
{
    // If not root, post init is immediate
    if(!m_isRoot) postInitProcess();
}

void RasterWindow::subClassRendering()
{
    for(Object* object : m_scene->getAllObjects()) {
        object->draw(m_scene);
    }

    m_HUD->render();
}

void RasterWindow::postInitProcess()
{
    initHUD();
}

void RasterWindow::initHUD()
{
    this->m_HUD = new HUD(getScreenWidth(), getScreenHeight());
}