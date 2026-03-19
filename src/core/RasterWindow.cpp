#include <ProjectIGAI/core/RasterWindow.hpp>

#include <ProjectIGAI/core/ImGuiWidgets.hpp>
#include <ProjectIGAI/graphics/CubemapManager.hpp>

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
    // Shadow map rendering
    glEnable(GL_CULL_FACE);
    DirectionalLight* sun = m_scene->getMainDirectionalLight(); 
    if (sun != nullptr) 
    {
        sun->computeLightSpaceMatrix();
        sun->beginShadowPass();
        
        Shader* shadowShader = sun->getShadowShader();

        for(Node* node : m_scene->getAllObjects()) {
            node->draw(m_scene, shadowShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_screenWidth, m_screenHeight);

        Shader* lightedShader = ShaderManager::getInstance().getResource("lighted");
        lightedShader->use();
        
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, sun->getDepthMap());
        lightedShader->setInt("shadowMap", 9);
        lightedShader->setMat4("lightSpaceMatrix", sun->getLightSpaceMatrix());
    }
    glDisable(GL_CULL_FACE);

    // Rendering skybox/cubemap
    if(m_scene->skyboxActive())
        CubemapManager::getInstance().drawCubemap(m_scene->skyboxName(), m_scene);

    for(Node* node : m_scene->getAllObjects()) {
        node->draw(m_scene);
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

    // FPS
    ImGui::Text((std::to_string(1.f / m_deltaTime) + " FPS").c_str());

    // Shader options
    bool tmpToneMapping = m_scene->toneMappingOn(), tmpGammaCorrection = m_scene->gammaCorrectionOn();
    if(ImGui::Checkbox("Tone mapping", &tmpToneMapping)) m_scene->setToneMappingActive(tmpToneMapping);
    if(ImGui::Checkbox("Gamma correction", &tmpGammaCorrection)) m_scene->setGammaCorrectionActive(tmpGammaCorrection);

    // Skybox
    ImGui::ColorEdit3("Background", m_scene->getBackgroundColorPointer());
    bool tmpSkyboxActive = m_scene->skyboxActive();
    if(ImGui::Checkbox("Skybox ?", &tmpSkyboxActive)) {
        m_scene->setSkyboxActive(tmpSkyboxActive);
    }
    ImGuiWidgets::mapSelector(CubemapManager::getInstance().getAll(), "Skybox", &m_scene->skyboxAttr(), &m_scene->skyboxName());
    ImGui::SliderFloat("Exposure", &CubemapManager::getInstance().getResource(m_scene->skyboxName())->exposure, 0.0f, 1.0f);

    // Rendering mode
    const char* items[] = {"PBR",
        "Normals", "Tangents", "UVs",
        "Albedo", "Roughness", "Metallic", "AO", "Height",
        "Shadow"};
    int tmpRenderingMode = m_scene->getRenderingMode();
    if(ImGui::Combo("Render mode", &tmpRenderingMode, items, IM_ARRAYSIZE(items))) {
        m_scene->setRenderingMode(tmpRenderingMode);
    }

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