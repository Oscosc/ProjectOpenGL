#include <ProjectIGAI/core/RasterWindow.hpp>

#include <format>
#include <iostream>

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

    ImGui::ColorEdit4("Background color", m_scene->getBackgroundColorPointer());
    if (ImGui::CollapsingHeader("Scene"))
    {
        // Point lights
        for(int i = 0; i < m_scene->lightsCount().x; i++) {
            ImGui::PushID(i);
            ImGui::Text("Point light %d", i);

            Light* light = m_scene->getLight(i, POINT_LIGHT_INDEX);

            glm::vec3 tmpColor = light->getLightMaterial().color;
            if(ImGui::ColorEdit3("Color", &tmpColor[0])) {
                m_scene->getLight(i, POINT_LIGHT_INDEX)->setColor(tmpColor);
            }

            float tmpIntensity = light->getLightMaterial().intensity;
            if(ImGui::SliderFloat("Intensity", &tmpIntensity, 0.0f, 100.0f)) {
                m_scene->getLight(i, POINT_LIGHT_INDEX)->setIntensity(tmpIntensity);
            }
            
            ImGui::Separator();
            ImGui::PopID();
        }

        // Directional lights
        for(int i = 0; i < m_scene->lightsCount().y; i++) {
            ImGui::PushID(i + m_scene->lightsCount().x);
            ImGui::Text("Directional light %d", i);

            Light* light = m_scene->getLight(i, DIR_LIGHT_INDEX);

            glm::vec3 tmpColor = light->getLightMaterial().color;
            if(ImGui::ColorEdit3("Color", &tmpColor[0])) {
                m_scene->getLight(i, DIR_LIGHT_INDEX)->setColor(tmpColor);
            }

            float tmpIntensity = light->getLightMaterial().intensity;
            if(ImGui::SliderFloat("Intensity", &tmpIntensity, 0.0f, 100.0f)) {
                m_scene->getLight(i, DIR_LIGHT_INDEX)->setIntensity(tmpIntensity);
            }
            
            ImGui::Separator();
            ImGui::PopID();
        }

        // Spot lights
        for(int i = 0; i < m_scene->lightsCount().z; i++) {
            ImGui::PushID(i + m_scene->lightsCount().x + m_scene->lightsCount().y);
            ImGui::Text("Spot light %d", i);

            Light* light = m_scene->getLight(i, SPOT_LIGHT_INDEX);

            glm::vec3 tmpColor = light->getLightMaterial().color;
            if(ImGui::ColorEdit3("Color", &tmpColor[0])) {
                m_scene->getLight(i, SPOT_LIGHT_INDEX)->setColor(tmpColor);
            }

            float tmpIntensity = light->getLightMaterial().intensity;
            if(ImGui::SliderFloat("Intensity", &tmpIntensity, 0.0f, 100.0f)) {
                m_scene->getLight(i, SPOT_LIGHT_INDEX)->setIntensity(tmpIntensity);
            }
            
            ImGui::Separator();
            ImGui::PopID();
        }
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