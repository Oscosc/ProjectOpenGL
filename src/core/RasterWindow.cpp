#include <ProjectIGAI/core/RasterWindow.hpp>

void RasterWindow::subClassRendering(Scene *scene)
{
    for(Object* object : scene->getAllObjects()) {
        object->draw(scene);
    }
}