#include <ProjectIGAI/core/RasterWindow.hpp>

#include <ProjectIGAI/core/ImGuiWidgets.hpp>

#include <extern/imgui/imgui.h>
#include <extern/imgui/backends/imgui_impl_glfw.h>
#include <extern/imgui/backends/imgui_impl_opengl3.h>

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
        object->getMaterial().shader->setInt("renderingMode", m_renderingMode);
    }

    m_HUD->render();
}

void RasterWindow::drawImGuiFrame()
{
    // Frame creation
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 700));

    // Modules part
    ImGui::Begin("Project IGAI configuration");

    ImGui::ColorEdit3("Background", m_scene->getBackgroundColorPointer());

    const char* items[] = {"PBR", "Normals", "UVs", "Texture"};
    ImGui::Combo("Render mode", &m_renderingMode, items, IM_ARRAYSIZE(items));

    if (ImGui::CollapsingHeader("Scene"))
    {
        ImGui::Indent();
        if (ImGui::CollapsingHeader("Lights")) {
            // Point lights
            ImGuiWidgets::pointLightsEditor(m_scene);

            // Directional lights
            ImGuiWidgets::dirLightsEditor(m_scene);

            // Spot lights
            ImGuiWidgets::spotLightsEditor(m_scene);
        }

        if(ImGui::CollapsingHeader("Objects")) {
            // Objects
            ImGuiWidgets::objectsEditor(m_scene);
        }
        ImGui::Unindent();
    }

    ImGui::End();
}

void RasterWindow::postInitProcess()
{
    initHUD();
}

void RasterWindow::initHUD()
{
    this->m_HUD = new HUD(getScreenWidth(), getScreenHeight());
}

void RasterWindow::onResize(int width, int height)
{
    // Calling base
    BaseWindow::onResize(width, height);

    // HUD update
    m_HUD->setScreenSize(width, height);
}