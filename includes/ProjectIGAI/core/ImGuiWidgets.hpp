#pragma once

#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

#include <extern/imgui/imgui.h>
#include <extern/imgui/backends/imgui_impl_glfw.h>
#include <extern/imgui/backends/imgui_impl_opengl3.h>

namespace ImGuiWidgets
{
    bool transformEditor(Transform& transform);

    bool lightMaterialEditor(glm::vec3& color, float& intensity);

    void objectsEditor(Scene* scene);

    void pointLightsEditor(Scene* scene);

    void dirLightsEditor(Scene* scene);
    
    void spotLightsEditor(Scene* scene);
}