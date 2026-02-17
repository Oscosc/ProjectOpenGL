#include <ProjectIGAI/graphics/GeometryManager.hpp>

#include <ProjectIGAI/geometry/GeometryFactory.hpp>
#include <ProjectIGAI/core/Logger.hpp>

Geometry *GeometryManager::getSphere(const float radius, const int segments)
{
    // Searching for existing object
    std::string key = "sphere_" + std::to_string(radius) + "_" + std::to_string(segments);
    if(m_geometries.find(key) != m_geometries.end()) {
        Logger::logPerf("Existing instance of '" + key + "' geometry found !");
        return m_geometries[key]; // Object already exists
    }

    // Creating object (when not found)
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indexes;

    GeometryFactory::createSphere(radius, segments, vertices, indexes);
    Geometry* newGeometry = new Geometry(vertices, indexes);

    // Storing and returning reference
    m_geometries[key] = newGeometry;
    return newGeometry;
}

Geometry *GeometryManager::getCube(const float halfLength)
{
    // Searching for existing object
    std::string key = "cube_" + std::to_string(halfLength);
    if(m_geometries.find(key) != m_geometries.end()) {
        Logger::logPerf("Existing instance of '" + key + "' geometry found !");
        return m_geometries[key]; // Object already exists
    }

    // Creating object (when not found)
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indexes;

    GeometryFactory::createCube(halfLength, vertices, indexes);
    Geometry* newGeometry = new Geometry(vertices, indexes);

    // Storing and returning reference
    m_geometries[key] = newGeometry;
    return newGeometry;
}

Geometry *GeometryManager::getMesh(const std::string path)
{
    // Searching for existing object
    std::string key = path; // Copy just for clarity
    if(m_geometries.find(key) != m_geometries.end()) {
        Logger::logPerf("Existing instance of '" + key + "' geometry found !");
        return m_geometries[key]; // Object already exists
    }

    // Creating object (when not found)
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indexes;

    GeometryFactory::loadFromObj(path, vertices, indexes);
    Geometry* newGeometry = new Geometry(vertices, indexes);

    // Storing and returning reference
    m_geometries[key] = newGeometry;
    return newGeometry;
}