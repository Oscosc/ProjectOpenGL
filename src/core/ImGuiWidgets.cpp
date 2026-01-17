#include <ProjectIGAI/core/ImGuiWidgets.hpp>

void ImGuiWidgets::pointLightsEditor(Scene *scene)
{
    for(int i = 0; i < scene->lightsCount().x; i++) {
        ImGui::PushID(i);
        ImGui::Text("Point light %d", i);

        Light* light = scene->getLight(i, POINT_LIGHT_INDEX);

        glm::vec3 tmpColor = light->getLightMaterial().color;
        if(ImGui::ColorEdit3("Color", &tmpColor[0])) {
            scene->getLight(i, POINT_LIGHT_INDEX)->setColor(tmpColor);
        }

        float tmpIntensity = light->getLightMaterial().intensity;
        if(ImGui::SliderFloat("Intensity", &tmpIntensity, 0.0f, 100.0f)) {
            scene->getLight(i, POINT_LIGHT_INDEX)->setIntensity(tmpIntensity);
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
}

void ImGuiWidgets::dirLightsEditor(Scene *scene)
{
    for(int i = 0; i < scene->lightsCount().y; i++) {
        ImGui::PushID(i + scene->lightsCount().x);
        ImGui::Text("Directional light %d", i);

        Light* light = scene->getLight(i, DIR_LIGHT_INDEX);

        glm::vec3 tmpColor = light->getLightMaterial().color;
        if(ImGui::ColorEdit3("Color", &tmpColor[0])) {
            scene->getLight(i, DIR_LIGHT_INDEX)->setColor(tmpColor);
        }

        float tmpIntensity = light->getLightMaterial().intensity;
        if(ImGui::SliderFloat("Intensity", &tmpIntensity, 0.0f, 100.0f)) {
            scene->getLight(i, DIR_LIGHT_INDEX)->setIntensity(tmpIntensity);
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
}

void ImGuiWidgets::spotLightsEditor(Scene *scene)
{
    for(int i = 0; i < scene->lightsCount().z; i++) {
        ImGui::PushID(i + scene->lightsCount().x + scene->lightsCount().y);
        ImGui::Text("Spot light %d", i);

        Light* light = scene->getLight(i, SPOT_LIGHT_INDEX);

        glm::vec3 tmpColor = light->getLightMaterial().color;
        if(ImGui::ColorEdit3("Color", &tmpColor[0])) {
            scene->getLight(i, SPOT_LIGHT_INDEX)->setColor(tmpColor);
        }

        float tmpIntensity = light->getLightMaterial().intensity;
        if(ImGui::SliderFloat("Intensity", &tmpIntensity, 0.0f, 100.0f)) {
            scene->getLight(i, SPOT_LIGHT_INDEX)->setIntensity(tmpIntensity);
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
}
