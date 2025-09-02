#include <ProjectIGAI/core/RasterWindow.hpp>

void RasterWindow::subClassRendering()
{
    for(Object* object : m_scene->getAllObjects()) {
        object->draw(m_scene);
    }
}