#pragma once

#include <unordered_map>
#include <string>

#include <ProjectIGAI/core/Singleton.hpp>

/**
 * @brief Structure for resource loading arguments.
 * 
 * All implementing classes must define a child structure
 * specific to itself.
 * 
 */
struct ResourceParam {
    std::string name;
    
    virtual ~ResourceParam() = default; // For polymorphism
};

template <typename Resource>
class IResourceManager
{
public:

    virtual ~IResourceManager() = default;

    /**
     * @brief Load a resource in the manager
     * 
     * @param params params used for this resource during loading.
     */
    virtual void loadResource(const ResourceParam& params) = 0;

    /**
     * @brief Gives a reference to the resource by it's name (null if not found)
     * 
     * @param name name of the resource defined when loaded
     */
    Resource* getResource(const std::string& name)
    {
        auto it = m_resources.find(name);
        if (it != m_resources.end()) {
            return &(it->second); 
        }
        return nullptr;
    }

    /**
     * @brief Get the All the Resources (for ImGui Combo mainly)
     */
    std::unordered_map<std::string, Resource>* getAll() { return &m_resources; }

protected:

    /**
     * @brief Default constructor (private because of singleton)
     */
    IResourceManager() = default;

    /** Resources storage map */
    std::unordered_map<std::string, Resource> m_resources;
};