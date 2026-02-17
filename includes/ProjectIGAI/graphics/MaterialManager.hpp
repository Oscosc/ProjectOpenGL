#pragma once

#include <map>
#include <string>
#include <iostream>
#include "ProjectIGAI/core/Singleton.hpp"
#include "ProjectIGAI/core/Logger.hpp"
#include "ProjectIGAI/graphics/Material.hpp"

class MaterialManager : public Singleton<MaterialManager>
{
    friend class Singleton<MaterialManager>;

public:

    ~MaterialManager() {
        for (auto& pair : m_materials) {
            delete pair.second;
        }
        m_materials.clear();
    }

    template<typename SubMatClass>
    SubMatClass* create(const std::string& name, bool& created = nullptr)
    {
        auto it = m_materials.find(name);
        if (it != m_materials.end()) {
            created = false;
            Logger::logWarning("Material '" + name + "' already exists. Returning existing instance.");
            
            SubMatClass* existing = dynamic_cast<SubMatClass*>(it->second);
            if (!existing) {
                Logger::logError("Material '" + name + "' exists but is NOT of the requested type!");
                return nullptr;
            }
            return existing;
        }

        created = true;
        SubMatClass* newMat = new SubMatClass();
        m_materials[name] = newMat;

        return newMat;
    }

    Material* getMaterial(const std::string& name)
    {
        auto it = m_materials.find(name);
        if (it != m_materials.end()) {
            return it->second;
        }
        
        Logger::logWarning("Material '" + name + "' does not exist.");
        return nullptr;
    }

private:

    MaterialManager() = default;

    std::map<std::string, Material*> m_materials;
};