#include <ProjectIGAI/core/ImGuiWidgets.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

bool ImGuiWidgets::transformEditor(Transform& transform)
{
    ImGui::Indent();

    bool changed = false;
    if (ImGui::CollapsingHeader("Transform")) {
        ImGui::Indent();

        changed |= ImGui::SliderFloat3("Position", glm::value_ptr(transform.position),  -10.0f,  10.0f);
        changed |= ImGui::SliderFloat3("Scale",    glm::value_ptr(transform.scale),       0.0f,  10.0f);
        changed |= ImGui::SliderFloat3("Rotation", glm::value_ptr(transform.rotation), -180.0f, 180.0f);
        
        ImGui::Unindent();
    }
    
    ImGui::Unindent();
    return changed;
}

bool ImGuiWidgets::lightMaterialEditor(glm::vec3 &color, float &intensity) 
{
    ImGui::Indent();

    bool changed = false;
    if (ImGui::CollapsingHeader("Light Material")) {
        ImGui::Indent();

        changed |= ImGui::ColorEdit3("Color", glm::value_ptr(color));
        changed |= ImGui::SliderFloat("Intensity", &intensity, 0.0f, 100.0f);

        ImGui::Unindent();
    }

    ImGui::Unindent();
    return changed;
}

void ImGuiWidgets::objectsEditor(Scene *scene)
{
    ImGui::PushID("Objects");

    std::vector<Object*> objects = scene->getAllObjects();
    for(int i = 0; i < objects.size(); i++) {

        ImGui::PushID(i);
        ImGui::Text("Object %d", i);

        Transform tmpTransform = objects.at(i)->getTransform();
        if(transformEditor(tmpTransform)) {
            objects.at(i)->setTransform(tmpTransform);
        }

        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::PopID();
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

        // TODO : Switch to separate block
        ImGui::Indent();
        if(ImGui::SliderFloat3("Position", glm::value_ptr(tmpPosition), -10.0f, 10.0f)) {
            light->setPosition(tmpPosition);
        }
        ImGui::Unindent();
        
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

        // TODO : Switch to separate block
        ImGui::Indent();
        if(ImGui::SliderFloat3("Direction", glm::value_ptr(tmpDirection), -10.0f, 10.0f)) {
            light->setDirection(tmpDirection);
        }
        ImGui::Unindent();
        
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

        // TODO : Switch to separate block
        ImGui::Indent();
        if(ImGui::SliderFloat3("Position", &tmpPosition[0], -10.0f, 10.0f)) {
            light->setPosition(tmpPosition);
        }
        ImGui::Unindent();

        // TODO : Switch to separate block
        ImGui::Indent();
        if(ImGui::SliderFloat3("Direction", &tmpDirection[0], -10.0f, 10.0f)) {
            light->setDirection(tmpDirection);
        }
        ImGui::Unindent();
        
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::PopID();
}