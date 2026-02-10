#pragma once

#include <unordered_map>

#include <ProjectIGAI/core/Scene.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

#include <extern/imgui/imgui.h>
#include <extern/imgui/backends/imgui_impl_glfw.h>
#include <extern/imgui/backends/imgui_impl_opengl3.h>

namespace ImGuiWidgets
{
    struct TransformFlags {
        bool showPosition = true;
        bool showScale    = true;
        bool showRotation = true;
    };

    #define OBJECT_FLAGS      {true,  true,  true}
    #define POINT_LIGHT_FLAGS {true,  false, false}
    #define DIR_LIGHT_FLAGS   {false, false, true}
    #define SPOT_LIGHT_FLAGS  {true,  false, true}

    bool transformEditor(Transform& transform, const TransformFlags flags);

    bool shaderMaterialEditor(ShaderMaterial& material);

    bool lightMaterialEditor(LightProperties& material);

    template<typename TFunc>
    bool genericEditor(const char* title, TFunc contentFunction)
    {
        if(!ImGui::CollapsingHeader(title))
            return false;
        
        ImGui::Indent();
        bool changed = contentFunction(); // Core
        ImGui::Unindent();

        return changed;
    }

    template<typename T>
    bool mapSelector(const std::unordered_map<std::string, T>* map, const std::string& title, int* selectedIndex)
    {
        std::vector<std::string> keys;
        keys.reserve(map->size());
        for(auto it = map->begin(); it != map->end(); ++it) { 
            keys.push_back(it->first); 
        }

        // Optionnal, for coherence
        std::sort(keys.begin(), keys.end());

        std::vector<const char*> key_ptrs;
        key_ptrs.reserve(keys.size());
        for (const auto& key : keys) {
            key_ptrs.push_back(key.c_str());
        }

        return ImGui::Combo(title.c_str(), selectedIndex, key_ptrs.data(), (int)key_ptrs.size());
    }

    void objectsEditor(Scene* scene);

    void pointLightsEditor(Scene* scene);

    void dirLightsEditor(Scene* scene);
    
    void spotLightsEditor(Scene* scene);
}