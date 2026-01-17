#pragma once

#include <ProjectIGAI/core/Scene.hpp>

#include <extern/imgui/imgui.h>
#include <extern/imgui/backends/imgui_impl_glfw.h>
#include <extern/imgui/backends/imgui_impl_opengl3.h>

namespace ImGuiWidgets
{
    void pointLightsEditor(Scene* scene);

    void dirLightsEditor(Scene* scene);
    
    void spotLightsEditor(Scene* scene);
}