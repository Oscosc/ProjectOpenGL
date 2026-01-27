#include <ProjectIGAI/core/ImGuiWidgets.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

bool ImGuiWidgets::transformEditor(glm::vec3 &position, glm::vec3 &scale, glm::vec3 &rotation)
{
    bool changed = false;
    if (ImGui::CollapsingHeader("Light Material")) {
        changed |= ImGui::SliderFloat3("Position", glm::value_ptr(position), -10.0f, 10.0f);
        changed |= ImGui::SliderFloat3("Scale",    glm::value_ptr(scale),    -10.0f, 10.0f);
        changed |= ImGui::SliderFloat3("Rotation", glm::value_ptr(rotation), -10.0f, 10.0f);
    }

    return changed;
}

bool ImGuiWidgets::lightMaterialEditor(glm::vec3 &color, float &intensity) 
{
    bool changed = false;
    if (ImGui::CollapsingHeader("Light Material")) {
        changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
        changed |= ImGui::SliderFloat("Intensity", &intensity, 0.0f, 100.0f);
    }

    return changed;
}

void ImGuiWidgets::pointLightsEditor(Scene *scene)
{
    ImGui::PushID("Category_PointLights");
    for(int i = 0; i < scene->lightsCount().x; i++) {

        ImGui::PushID(i);
        ImGui::Text("Point light %d", i);

        PointLight* light = static_cast<PointLight*>(scene->getLight(i, POINT_LIGHT_INDEX));

        glm::vec3 tmpColor = light->getLightMaterial().color;
        float tmpIntensity = light->getLightMaterial().intensity;
        glm::vec3 tmpPosition = light->getPosition();

        if(lightMaterialEditor(tmpColor, tmpIntensity)) {
            light->setColor(tmpColor);
            light->setIntensity(tmpIntensity);
        }

        if(ImGui::SliderFloat3("Position", glm::value_ptr(tmpPosition), -10.0f, 10.0f)) {
            light->setPosition(tmpPosition);
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::PopID();
}

void ImGuiWidgets::dirLightsEditor(Scene *scene)
{
    ImGui::PushID("Category_DirectionalLights");
    for(int i = 0; i < scene->lightsCount().y; i++) {

        ImGui::PushID(i);
        ImGui::Text("Directional light %d", i);

        DirectionalLight* light = static_cast<DirectionalLight*>(scene->getLight(i, DIR_LIGHT_INDEX));

        glm::vec3 tmpColor = light->getLightMaterial().color;
        float tmpIntensity = light->getLightMaterial().intensity;
        glm::vec3 tmpDirection = light->getDirection();

        if(lightMaterialEditor(tmpColor, tmpIntensity)) {
            light->setColor(tmpColor);
            light->setIntensity(tmpIntensity);
        }

        if(ImGui::SliderFloat3("Direction", glm::value_ptr(tmpDirection), -10.0f, 10.0f)) {
            light->setDirection(tmpDirection);
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::PopID();
}

void ImGuiWidgets::spotLightsEditor(Scene *scene)
{
    ImGui::PushID("Category_SpotLights");
    for(int i = 0; i < scene->lightsCount().z; i++) {

        ImGui::PushID(i);
        ImGui::Text("Spot light %d", i);

        SpotLight* light = static_cast<SpotLight*>(scene->getLight(i, SPOT_LIGHT_INDEX));

        glm::vec3 tmpColor = light->getLightMaterial().color;
        float tmpIntensity = light->getLightMaterial().intensity;
        glm::vec3 tmpPosition = light->getPosition();
        glm::vec3 tmpDirection = light->getDirection();

        if(lightMaterialEditor(tmpColor, tmpIntensity)) {
            light->setColor(tmpColor);
            light->setIntensity(tmpIntensity);
        }

        if(ImGui::SliderFloat3("Position", &tmpPosition[0], -10.0f, 10.0f)) {
            light->setPosition(tmpPosition);
        }

        if(ImGui::SliderFloat3("Direction", &tmpDirection[0], -10.0f, 10.0f)) {
            light->setDirection(tmpDirection);
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::PopID();
}