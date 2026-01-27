#pragma once

#include <ProjectIGAI/core/Scene.hpp>

#include <extern/imgui/imgui.h>
#include <extern/imgui/backends/imgui_impl_glfw.h>
#include <extern/imgui/backends/imgui_impl_opengl3.h>

class Object;

namespace ImGuiWidgets
{
    bool transformEditor(glm::vec3& position, glm::vec3& scale, glm::vec3& rotation);

    bool lightMaterialEditor(glm::vec3& color, float& intensity);

    void pointLightsEditor(Scene* scene);

    void dirLightsEditor(Scene* scene);
    
    void spotLightsEditor(Scene* scene);
}